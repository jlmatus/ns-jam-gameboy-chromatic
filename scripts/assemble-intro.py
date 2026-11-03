#!/usr/bin/env python3
"""Assemble supplied intro assets/timeline into editable GB Studio resources.

Never renders or modifies source images, palettes, or music. Actor motion is
sampled from the timeline into native Set Position events, avoiding unsigned
Move To targets and its equal-speed-per-axis diagonal path. Native actors use
the same canvas anchor for both sprite sizes: timeline (x, y) + (0, 8).
"""
import argparse
import hashlib
import json
import shutil
import uuid
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
GAME = ROOT / "game"
SOURCE = ROOT / "intro"
TIMELINE = json.loads((SOURCE / "timeline.json").read_text())
TITLE = "12f07015-0a42-504a-8d39-3f689e13ad3a"
NAMESPACE = uuid.UUID("235f0da7-6f57-4833-930b-9500e9a554ca")
ANCHOR = (0, 8)


def uid(name):
    return str(uuid.uuid5(NAMESPACE, name))


def write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n")


def number(n):
    return {"type": "number", "value": n}


def event(command, args=None, children=None):
    # Stable event IDs make regeneration reviewable.
    event.sequence += 1
    result = {"id": uid(f"event/{event.owner}/{event.sequence}"), "command": command}
    if args is not None:
        result["args"] = args
    if children is not None:
        result["children"] = children
    return result


event.sequence = 0
event.owner = ""


def wait(frames):
    return event("EVENT_WAIT", {"units": "frames", "frames": number(frames)})


def cue_stamp(first=False):
    # hUGEDriver initializes before its first four-tick row. The first row
    # begins four VBlanks after Play Music; later stamps come from routine 0.
    code = ".globl _sys_time\nVM_GET_INT16 VAR_VARIABLE_7, _sys_time"
    if first:
        code += "\nVM_RPN\n.R_REF VAR_VARIABLE_7\n.R_INT16 4\n.R_OPERATOR .ADD\n.R_REF_SET VAR_VARIABLE_7\n.R_STOP"
    return event("EVENT_GBVM_SCRIPT", {"script": code})


def deadline(frame):
    # A per-context stack slot avoids sharing scratch RAM between actors.
    label = f"{10000 + event.sequence * 2}$"
    ready = f"{10001 + event.sequence * 2}$"
    code = f""".globl _sys_time
VM_PUSH_CONST 0
{label}:
VM_GET_INT16 .ARG0, _sys_time
VM_RPN
.R_REF .ARG0
.R_REF VAR_VARIABLE_7
.R_OPERATOR .SUB
.R_REF_SET .ARG0
.R_STOP
VM_IF_CONST .GTE, .ARG0, {frame}, {ready}, 0
VM_IDLE
VM_JUMP {label}
{ready}:
VM_POP 1"""
    return event("EVENT_GBVM_SCRIPT", {"script": code})


def shake(step):
    # A scene can finish loading after the flash's shake has already elapsed.
    # Invoke only the remaining interval, keeping later palette changes on cue.
    done = f"{30000 + event.sequence}$"
    direction = {"horizontal": 1, "vertical": 2, "diagonal": 3}[step["direction"]]
    code = f""".globl _sys_time, b_camera_shake_frames, _camera_shake_frames
VM_PUSH_CONST 0
VM_GET_INT16 .ARG0, _sys_time
VM_RPN
.R_INT16 {step['at'] + step['frames']}
.R_REF .ARG0
.R_REF VAR_VARIABLE_7
.R_OPERATOR .SUB
.R_OPERATOR .SUB
.R_REF_SET .ARG0
.R_STOP
VM_IF_CONST .LTE, .ARG0, 0, {done}, 0
VM_PUSH_CONST {direction}
VM_PUSH_CONST {step['magnitude']}
VM_INVOKE b_camera_shake_frames, _camera_shake_frames, 0, .ARG2
VM_POP 2
{done}:
VM_POP 1"""
    return event("EVENT_GBVM_SCRIPT", {"script": code})


def switch(destination):
    target = next((s for s in TIMELINE["shots"] if uid(s["scene"]) == destination), None)
    landing = ((target["camera_start"][0] + 72) / 8,
               (target["camera_start"][1] + 64) / 8) if target else (9, 9)
    return event("EVENT_SWITCH_SCENE", {
        "sceneId": destination, "x": number(landing[0]), "y": number(landing[1]),
        "direction": "down", "fadeSpeed": 0,
    })


def end_intro():
    return [event("EVENT_MUSIC_STOP"), switch(TITLE)]


def position(actor_id, x, y):
    return event("EVENT_ACTOR_SET_POSITION", {
        "actorId": actor_id, "units": "pixels",
        "x": number(x + ANCHOR[0]), "y": number(y + ANCHOR[1]),
    })


def copy_inputs():
    checksums = {}
    for base, dest in [(SOURCE / "gbstudio/assets", GAME / "assets"),
                       (SOURCE / "gbstudio/project/palettes", GAME / "project/palettes")]:
        for source in sorted(base.rglob("*")):
            if not source.is_file():
                continue
            target = dest / source.relative_to(base)
            if target.exists() and target.read_bytes() != source.read_bytes():
                raise RuntimeError(f"Refusing to overwrite different supplied asset: {target}")
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source, target)
            digest = hashlib.sha256(source.read_bytes()).hexdigest()
            assert hashlib.sha256(target.read_bytes()).hexdigest() == digest
            checksums[str(target.relative_to(ROOT))] = digest
    return checksums


def sprite(name, size, palette_index):
    """Author native frame/tile metadata referencing original PNG pixels 1:1."""
    source = GAME / "assets/sprites" / f"{name}.png"
    image = Image.open(source).convert("RGB")
    transparent = (101, 255, 0)
    frames = []
    for frame in range(image.width // size):
        # Select occupied columns at native pixel coordinates; 22px ball uses
        # three 8px-wide hardware objects, rather than four grid-aligned ones.
        occupied = [(x, y) for y in range(size) for x in range(size)
                    if image.getpixel((frame * size + x, y)) != transparent]
        left = min(x for x, y in occupied)
        right = max(x for x, y in occupied)
        tiles = []
        for y in range(0, size, 16):
            for x in range(left, right + 1, 8):
                if not any(image.getpixel((frame * size + xx, yy)) != transparent
                           for yy in range(y, min(y + 16, size))
                           for xx in range(x, min(x + 8, size))):
                    continue
                tiles.append({
                    "id": uid(f"{name}/{frame}/{x}/{y}"), "x": x - (size // 2 - 8), "y": size - 16 - y,
                    "sliceX": frame * size + x, "sliceY": y,
                    "flipX": False, "flipY": False, "palette": 0,
                    "paletteIndex": palette_index, "objPalette": "OBP0", "priority": False,
                })
        frames.append({"id": uid(f"{name}/frame/{frame}"), "tiles": tiles})
    animations = [{"id": uid(f"{name}/animation/{i}"),
                   "frames": frames if i == 0 else [{"id": uid(f"{name}/empty/{i}"), "tiles": []}]}
                  for i in range(8)]
    write(source.with_suffix(".png.gbsres"), {
        "_resourceType": "sprite", "id": uid(name), "name": name, "symbol": f"sprite_{name}",
        "states": [{"id": uid(f"{name}/state"), "name": "", "animationType": "fixed",
                    "flipLeft": False, "animations": animations}],
        "numTiles": sum(len(f["tiles"]) * 2 for f in frames),
        "canvasOriginX": 8 - size // 2, "canvasOriginY": 16 - size, "canvasWidth": size, "canvasHeight": size,
        "boundsX": 0, "boundsY": -8, "boundsWidth": size, "boundsHeight": size,
        "animSpeed": 255, "filename": source.name, "width": image.width, "height": image.height,
        "checksum": hashlib.sha1(source.read_bytes()).hexdigest(),
    })


def camera_positions(shot):
    positions = [tuple(shot["camera_start"])] * (shot["length_frames"] + 1)
    current = tuple(shot["camera_start"])
    for step in shot["script"]:
        at = step.get("at", 0)
        if step["op"] == "camera_set_position":
            current = (step["x"], step["y"])
            for f in range(at, len(positions)):
                positions[f] = current
        elif step["op"] == "camera_move_to":
            start = current
            target = (step["x"], step["y"])
            speed = step["speed_px_per_frame"]
            for f in range(at, len(positions)):
                positions[f] = tuple(round(a + max(-speed*(f-at), min(speed*(f-at), b-a)))
                                     for a, b in zip(start, target))
            current = target
    return positions


def actor_script(shot, actor, actor_id, sample_only=False):
    x, y = actor["start_position"]
    visible, frame, at = True, 0, 0
    samples = {}
    for step in actor["script"]:
        op = step["op"]
        if op == "hide":
            visible = False
        elif op == "show":
            visible = True
        elif op == "set_frame":
            frame = step["frame"]
        elif op == "set_position":
            x, y = step["x"], step["y"]
        elif op == "wait":
            for f in range(step["frames"]):
                samples[at + f] = (x, y, visible, frame)
            at += step["frames"]
        elif op == "move_to":
            start_x, start_y = x, y
            duration = step["frames"]
            for f in range(duration):
                # Source timeline segments specify duration and endpoints. Keep
                # that straight path, independently of stock Move To semantics.
                samples[at + f] = (round(start_x + (step["x"] - start_x) * f / duration),
                                   round(start_y + (step["y"] - start_y) * f / duration),
                                   visible, frame)
            at += duration
            x, y = step["x"], step["y"]
        else:
            raise RuntimeError(f"Unknown actor operation: {op}")
        samples[at] = (x, y, visible, frame)
    cameras = camera_positions(shot)
    size = 32 if actor["sprite"] == "intro_ball" else 16
    if sample_only:
        states = []
        for f in range(shot["length_frames"]):
            xx, yy, shown, cel = samples.get(f, (x, y, visible, frame))
            cx, cy = cameras[f]
            shown = shown and xx + size > cx - 6 and xx < cx + 166 and yy + size > cy - 6 and yy < cy + 150
            states.append((xx, yy, shown, cel))
        return states
    last = None
    result = []
    for f in range(shot["length_frames"]):
        state = samples.get(f, (x, y, visible, frame))
        xx, yy, shown, cel = state
        cx, cy = cameras[f]
        # Persistent actors still consume OAM offscreen. Hide fully clipped
        # canvases so 8-bit hardware positions cannot wrap back into view.
        shown = shown and xx + size > cx - 6 and xx < cx + 166 and yy + size > cy - 6 and yy < cy + 150
        state = (xx, yy, shown, cel)
        if state != last:
            result.append(deadline(f))
            result.append(position(actor_id, xx, yy))
            if last is None or cel != last[3]:
                result.append(event("EVENT_ACTOR_SET_FRAME", {"actorId": actor_id, "frame": number(cel)}))
            if last is None or shown != last[2]:
                if shown:
                    result.append(event("EVENT_ACTOR_ACTIVATE", {"actorId": actor_id}))
                result.append(event("EVENT_ACTOR_SHOW" if shown else "EVENT_ACTOR_HIDE", {"actorId": actor_id}))
            last = state
    result.append(event("EVENT_STOP"))
    return result


def choreography(shot):
    """One shared timeline avoids ten concurrent per-frame polling loops.

    Bank-sized background-script chunks keep each native script below 16 KiB.
    All commands address native actors; no engine or image code is replaced.
    """
    all_states = [actor_script(shot, a, uid(f"{shot['scene']}/{a['id']}"), True)
                  for a in shot["actors"]]
    chunks = []
    for start in range(0, shot["length_frames"], 24):
        chunk = []
        for f in range(start, min(start + 24, shot["length_frames"])):
            commands = []
            for index, states in enumerate(all_states):
                x, y, shown, cel = states[f]
                if shown:
                    # GB Studio 4.2's packed actor ABI: 56 bytes, flags at 0,
                    # x at 1, y at 3. Three GBVM writes avoid banked helper
                    # overhead for ten simultaneous shards. check-intro.py
                    # verifies the linked array size before running this ROM.
                    base = (index + 1) * 56
                    commands += [".globl _actors",
                                 f"VM_SET_CONST_INT16 ^/(_actors + {base + 1})/, {(x + ANCHOR[0]) * 32 & 65535}",
                                 f"VM_SET_CONST_INT16 ^/(_actors + {base + 3})/, {(y + ANCHOR[1]) * 32 & 65535}",
                                 f"VM_SET_CONST_INT8 ^/(_actors + {base})/, 48"]
                else:
                    commands += [".globl _actors",
                                 f"VM_SET_CONST_INT8 ^/(_actors + {(index + 1) * 56})/, 50"]
            if commands:
                skip = f"{40000 + event.sequence}$"
                # Never replay obsolete frames after a scene load. One complete
                # current-state snapshot lets rendering/input run immediately.
                guard = f""".globl _sys_time
VM_PUSH_CONST 0
VM_GET_INT16 .ARG0, _sys_time
VM_RPN
.R_REF .ARG0
.R_REF VAR_VARIABLE_7
.R_OPERATOR .SUB
.R_REF_SET .ARG0
.R_STOP
VM_IF_CONST .GT, .ARG0, {f + 3}, {skip}, 1"""
                chunk += [deadline(f), event("EVENT_GBVM_SCRIPT", {
                    "script": guard + "\n" + "\n".join(commands) + f"\nVM_IDLE\n{skip}:"})]
        if chunk:
            chunks.append(chunk)
    following = []
    for chunk in reversed(chunks):
        if following:
            chunk.append(event("EVENT_THREAD_START", {"variable": "T1"}, {"true": following}))
        following = chunk
    return following


def scene_script(shot):
    result = []
    for step in shot["script"]:
        op = step["op"]
        if op == "wait":
            continue
        result.append(deadline(step["at"]))
        if op in ("set_background_palettes", "set_sprite_palettes"):
            result.append(event("EVENT_PALETTE_SET_BACKGROUND" if op == "set_background_palettes"
                                else "EVENT_PALETTE_SET_SPRITE",
                                {f"palette{i}": step["slots"].get(str(i), "keep") for i in range(8)}))
        elif op in ("camera_set_position", "camera_move_to"):
            result.append(event("EVENT_CAMERA_SET_POSITION" if op == "camera_set_position" else "EVENT_CAMERA_MOVE_TO",
                                {"x": number(step["x"]), "y": number(step["y"]), "units": "pixels",
                                 **({"speed": step["speed_px_per_frame"]} if op == "camera_move_to" else {})}))
        elif op == "camera_shake":
            result.append(shake(step))
        elif op == "fade_out":
            # Advance the stock five-step palette fade on exact cue deadlines.
            # Nonmodal updates keep input and music callbacks responsive.
            result.append(event("EVENT_GBVM_SCRIPT", {"script":
                ".globl _fade_frames_per_step\nVM_SET_CONST_INT8 _fade_frames_per_step, 0\nVM_FADE_OUT 0"}))
            for level in range(1, 6):
                result.append(deadline(step["at"] + round(step["frames"] * level / 5)))
                result.append(event("EVENT_GBVM_SCRIPT", {"script":
                    ".globl b_fade_update, _fade_update\nVM_CALL_NATIVE b_fade_update, _fade_update"}))
        elif op == "end_intro":
            result += end_intro()
        else:
            raise RuntimeError(f"Unknown scene operation: {op}")
    return result


def assemble(count):
    checksums = copy_inputs()
    sprite("intro_ball", 32, 0)
    sprite("intro_shard", 16, 1)
    # A genuinely empty player frame prevents the engine's pre-init scene
    # render from showing the hidden player for one frame during a hard cut.
    player_png = GAME / "assets/sprites/intro_hidden_player.png"
    shutil.copyfile(GAME / "assets/sprites/intro_ball.png", player_png)
    hidden = json.loads((GAME / "assets/sprites/intro_ball.png.gbsres").read_text())
    hidden.update(id=uid("intro_hidden_player"), name="intro_hidden_player",
                  symbol="sprite_intro_hidden_player", filename=player_png.name, numTiles=0)
    for state in hidden["states"]:
        state["id"] = uid("intro_hidden_player/state")
        for i, animation in enumerate(state["animations"]):
            animation["id"] = uid(f"intro_hidden_player/anim/{i}")
            animation["frames"] = [{"id": uid(f"intro_hidden_player/frame/{i}"), "tiles": []}]
    write(player_png.with_suffix(".png.gbsres"), hidden)
    music_id = uid("ns_jam_intro")
    write(GAME / "assets/music/ns_jam_intro.uge.gbsres", {
        "_resourceType": "music", "id": music_id, "name": "ns_jam_intro",
        "symbol": "music_ns_jam_intro", "filename": "ns_jam_intro.uge", "type": "uge", "settings": {},
    })
    for shot in TIMELINE["shots"][:count]:
        name = shot["scene"]
        event.owner, event.sequence = name, 0
        destination = uid(TIMELINE["shots"][shot["shot"]]["scene"]) if shot["shot"] < count else TITLE
        next_shot = TIMELINE["shots"][shot["shot"]] if shot["shot"] < count else None
        cut = [cue_stamp()]
        if next_shot and len(set(next_shot["palette_ids"])) == 1:
            cut.append(event("EVENT_PALETTE_SET_BACKGROUND", {
                f"palette{i}": palette for i, palette in enumerate(next_shot["palette_ids"])
            }))
        cut.append(switch(destination))
        init = [event("EVENT_ACTOR_HIDE", {"actorId": "player"}),
                event("EVENT_OVERLAY_HIDE"),
                event("EVENT_CAMERA_SET_POSITION", {"x": number(shot["camera_start"][0]),
                                                   "y": number(shot["camera_start"][1]), "units": "pixels"}),
                event("EVENT_ENGINE_FIELD_SET", {"engineFieldKey": "fade_style", "value": number(1)}),
                event("EVENT_GBVM_SCRIPT", {"script": ".globl _fade_frames_per_step, _fade_timer, _fade_running\nVM_SET_CONST_INT8 _fade_frames_per_step, 0\nVM_SET_CONST_INT8 _fade_timer, 0\nVM_SET_CONST_INT8 _fade_running, 0\nVM_FADE_IN 0"}),
                event("EVENT_SET_MUSIC_ROUTINE", {"routine": 0}, {"true":
                      cut if destination != TITLE else end_intro()}),
                event("EVENT_SET_INPUT_SCRIPT", {"input": ["a", "start"], "override": True}, {"true": end_intro()})]
        for actor in shot["actors"]:
            actor_id = uid(f"{name}/{actor['id']}")
            cel = actor_script(shot, actor, actor_id, True)[0][3]
            init += [event("EVENT_ACTOR_ACTIVATE", {"actorId": actor_id}),
                     event("EVENT_ACTOR_HIDE", {"actorId": actor_id}),
                     event("EVENT_ACTOR_SET_FRAME", {"actorId": actor_id, "frame": number(cel)})]
        if shot["shot"] != 1:
            init.append(event("EVENT_IF_INPUT", {"input": ["a", "start"], "__disableElse": True},
                              {"true": end_intro(), "false": []}))
        if shot["shot"] == 1:
            init.append(event("EVENT_SET_VALUE", {"variable": "7", "value": number(0)}))
            init.append(event("EVENT_MUSIC_PLAY", {"musicId": music_id}))
            init.append(cue_stamp(first=True))
        main_script = scene_script(shot)
        if shot["actors"]:
            init.append(event("EVENT_THREAD_START", {"variable": "T1"}, {"true": choreography(shot)}))
        else:
            init.append(event("EVENT_SCRIPT_UNLOCK"))
        init += main_script
        folder = GAME / "project/scenes" / name
        write(folder / "scene.gbsres", {
            "_resourceType": "scene", "id": uid(name), "_index": shot["shot"],
            "type": "TOPDOWN", "name": name, "symbol": f"scene_{name}",
            "x": 520 + ((shot["shot"] - 1) % 4) * 560, "y": ((shot["shot"] - 1) // 4) * 360,
            "width": shot["size_px"][0] // 8, "height": shot["size_px"][1] // 8,
            "backgroundId": shot["background_id"], "tilesetId": "", "colorModeOverride": "none",
            "paletteIds": shot["palette_ids"],
            "spritePaletteIds": [TIMELINE["sprite_palettes"]["ball"], TIMELINE["sprite_palettes"]["shard"]],
            "autoFadeSpeed": None, "script": init,
            "playerHit1Script": [], "playerHit2Script": [], "playerHit3Script": [], "collisions": "",
            "playerSpriteSheetId": uid("intro_hidden_player"),
        })
        for index, actor in enumerate(shot["actors"]):
            actor_id = uid(f"{name}/{actor['id']}")
            write(folder / "actors" / f"{actor['id']}.gbsres", {
                "_resourceType": "actor", "id": actor_id, "_index": index,
                "symbol": f"actor_{name}_{actor['id']}", "prefabId": "", "name": actor["id"],
                "coordinateType": "pixels", "x": actor["start_position"][0] + ANCHOR[0],
                "y": actor["start_position"][1] + ANCHOR[1], "frame": 0, "animate": False,
                "spriteSheetId": uid(actor["sprite"]), "paletteId": "", "direction": "down",
                "moveSpeed": 1, "animSpeed": 255, "isPinned": False, "persistent": True,
                "collisionGroup": "", "collisionExtraFlags": [], "prefabScriptOverrides": {},
                "script": [], "startScript": [], "updateScript": [],
                "hit1Script": [], "hit2Script": [], "hit3Script": [],
            })
    settings_path = GAME / "project/settings.gbsres"
    settings = json.loads(settings_path.read_text())
    settings.update(startSceneId=uid(TIMELINE["shots"][0]["scene"]), startX=52, startY=9)
    write(settings_path, settings)
    for path in (GAME / "project/scenes").glob("*/scene.gbsres"):
        title = json.loads(path.read_text())
        if title["id"] != TITLE:
            continue
        event.owner, event.sequence = "intro/title-entry", 0
        reset = event("EVENT_GBVM_SCRIPT", {"script":
            ".globl _fade_frames_per_step, _fade_timer, _fade_running\n"
            "VM_SET_CONST_INT8 _fade_frames_per_step, 0\n"
            "VM_SET_CONST_INT8 _fade_timer, 0\n"
            "VM_SET_CONST_INT8 _fade_running, 0\nVM_FADE_IN 0"})
        title["autoFadeSpeed"] = None
        title["script"] = [reset] + [e for e in title["script"] if e["id"] != reset["id"]]
        write(path, title)
    write(GAME / "build/intro-assembly.json", {
        "shots": count, "actor_anchor_px": ANCHOR, "source_sha256": checksums,
        "scenes": {s["scene"]: uid(s["scene"]) for s in TIMELINE["shots"][:count]},
    })
    print(f"Assembled {count} scenes; copied {len(checksums)} input files byte-for-byte; actor anchor {ANCHOR}.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--shots", type=int, choices=(4, 14), default=14)
    assemble(parser.parse_args().shots)

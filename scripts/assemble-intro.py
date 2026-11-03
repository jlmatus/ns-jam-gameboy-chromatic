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


def actor_script(shot, actor, actor_id):
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
            result.append(event("EVENT_CAMERA_SHAKE", {"units": "frames", "frames": step["frames"],
                                "magnitude": number(step["magnitude"]), "shakeDirection": step["direction"]}))
        elif op == "fade_out":
            # Native 5-step fades: speed 2 ~=24 frames, speed 3 ~=48.
            result.append(event("EVENT_FADE_OUT", {"speed": "2" if step["frames"] < 40 else "3"}))
        elif op == "end_intro":
            result += end_intro()
        else:
            raise RuntimeError(f"Unknown scene operation: {op}")
    return result


def assemble(count):
    checksums = copy_inputs()
    sprite("intro_ball", 32, 0)
    sprite("intro_shard", 16, 1)
    music_id = uid("ns_jam_intro")
    write(GAME / "assets/music/ns_jam_intro.uge.gbsres", {
        "_resourceType": "music", "id": music_id, "name": "ns_jam_intro",
        "symbol": "music_ns_jam_intro", "filename": "ns_jam_intro.uge", "type": "uge", "settings": {},
    })
    for shot in TIMELINE["shots"][:count]:
        name = shot["scene"]
        event.owner, event.sequence = name, 0
        destination = uid(TIMELINE["shots"][shot["shot"]]["scene"]) if shot["shot"] < count else TITLE
        init = [event("EVENT_ACTOR_HIDE", {"actorId": "player"}),
                event("EVENT_CAMERA_SET_POSITION", {"x": number(shot["camera_start"][0]),
                                                   "y": number(shot["camera_start"][1]), "units": "pixels"}),
                event("EVENT_ENGINE_FIELD_SET", {"engineFieldKey": "fade_style", "value": number(1)}),
                event("EVENT_GBVM_SCRIPT", {"script": ".globl _fade_frames_per_step, _fade_timer, _fade_running\nVM_SET_CONST_INT8 _fade_frames_per_step, 0\nVM_SET_CONST_INT8 _fade_timer, 0\nVM_SET_CONST_INT8 _fade_running, 0\nVM_FADE_IN 0"}),
                event("EVENT_SET_MUSIC_ROUTINE", {"routine": 0}, {"true":
                      [cue_stamp(), switch(destination)] if destination != TITLE else end_intro()}),
                event("EVENT_SET_INPUT_SCRIPT", {"input": ["a", "start"], "override": True}, {"true": end_intro()})]
        if shot["shot"] == 1:
            init.append(event("EVENT_SET_VALUE", {"variable": "7", "value": number(0)}))
            init.append(event("EVENT_MUSIC_PLAY", {"musicId": music_id}))
            init.append(cue_stamp(first=True))
        main_script = scene_script(shot)
        init.append(event("EVENT_THREAD_START", {"variable": "T0"}, {"true": main_script}))
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
            "playerSpriteSheetId": uid("intro_ball"),
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
                "script": [], "startScript": [], "updateScript": actor_script(shot, actor, actor_id),
                "hit1Script": [], "hit2Script": [], "hit3Script": [],
            })
    settings_path = GAME / "project/settings.gbsres"
    settings = json.loads(settings_path.read_text())
    settings.update(startSceneId=uid(TIMELINE["shots"][0]["scene"]), startX=52, startY=9)
    write(settings_path, settings)
    write(GAME / "build/intro-assembly.json", {
        "shots": count, "actor_anchor_px": ANCHOR, "source_sha256": checksums,
        "scenes": {s["scene"]: uid(s["scene"]) for s in TIMELINE["shots"][:count]},
    })
    print(f"Assembled {count} scenes; copied {len(checksums)} input files byte-for-byte; actor anchor {ANCHOR}.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--shots", type=int, choices=(4, 14), default=14)
    assemble(parser.parse_args().shots)

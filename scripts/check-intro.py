#!/usr/bin/env python3
"""Capture the exact cartridge beside supplied video frames, with native logs."""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

from PIL import Image, ImageDraw
from pyboy import PyBoy

ROOT = Path(__file__).resolve().parents[1]
FPS = 4194304 / 70224
TIMES = [0.7, 2.5, 4.1, 6.5, 9.6, 11.6, 12.3, 12.6, 16.8, 18.3, 19.4, 21.6, 23.1, 25.1, 26.2]


def symbols(path):
    return {name: int(value, 16) for name, value in re.findall(r"^DEF (\S+) (0x[0-9A-Fa-f]+)", path.read_text(), re.M)}


def capture(rom, output, seconds):
    output.mkdir(parents=True, exist_ok=True)
    sym = symbols(Path(str(rom) + ".debug/symbols.noi"))
    scene_symbols = {value: name[7:] for name, value in sym.items()
                     if name.startswith("_scene_") and not name.endswith(("_actors", "_collisions", "_sprites", "_init"))}
    pyboy = PyBoy(str(rom), window="null", cgb=True, sound_emulated=False)
    pyboy.set_emulation_speed(0)
    def byte(name):
        return pyboy.memory[sym[name]]
    def word(name):
        address = sym[name]
        return pyboy.memory[address] | pyboy.memory[address + 1] << 8
    def scene():
        address = sym["_current_scene"]
        value = pyboy.memory[address] << 16 | pyboy.memory[address + 1] | pyboy.memory[address + 2] << 8
        return scene_symbols.get(value, f"unknown_{value:06x}")
    rows, cuts, samples = [], [], []
    previous_scene = None
    origin = None
    frames_dir = ROOT / "game/build/intro-captures" / output.name
    frames_dir.mkdir(parents=True, exist_ok=True)
    wanted = {}
    for frame in range(1, round((seconds + 3) * FPS)):
        pyboy.tick(1, render=True, sound=False)
        current_scene = scene()
        if origin is None and current_scene == "intro_01_court" and byte("_music_current_track_bank") not in (0, 255):
            # The driver initializes, then starts row 0 four VBlanks later.
            origin = frame + 4
            wanted = {origin + round(t * FPS): t for t in TIMES if t < seconds}
        if origin is None:
            continue
        image = pyboy.screen.image.convert("RGB")
        image.save(frames_dir / f"{frame:05d}.png")
        colors = image.getcolors(23040)
        uniform = colors[0][1] if colors and len(colors) == 1 else None
        row = {"frame": frame, "intro_frame": frame - origin,
               "time_s": round((frame - origin) / FPS, 6), "scene": current_scene,
               "camera": [word("_scroll_x"), word("_scroll_y")],
               "fade_running": byte("_fade_running"), "music_bank": byte("_music_current_track_bank"),
               "uniform_rgb": uniform, "rgba_sha256": hashlib.sha256(pyboy.screen.ndarray.tobytes()).hexdigest()}
        rows.append(row)
        if current_scene != previous_scene:
            cuts.append(row.copy())
            previous_scene = current_scene
        if frame in wanted:
            time = wanted[frame]
            slug = str(time).replace(".", "_")
            native_path = output / f"rom-{slug}.png"
            image.save(native_path)
            ref_path = output / f"reference-{slug}.png"
            subprocess.run(["ffmpeg", "-y", "-v", "error", "-ss", str(time), "-i",
                            str(ROOT / "intro/reference/ns_jam_intro_reference.mp4"),
                            "-frames:v", "1", str(ref_path)], check=True)
            reference = Image.open(ref_path).convert("RGB")
            native4 = image.resize((640, 576), Image.Resampling.NEAREST)
            pair = Image.new("RGB", (1288, 610), "#18232e")
            draw = ImageDraw.Draw(pair)
            draw.text((10, 10), f"REFERENCE  {time:.1f}s", fill="white")
            draw.text((654, 10), f"ROM  {time:.1f}s  / frame {frame}", fill="white")
            pair.paste(reference, (0, 30))
            pair.paste(native4, (648, 30))
            pair_path = output / f"pair-{slug}.png"
            pair.save(pair_path)
            samples.append({"time_s": time, "frame": frame, "scene": current_scene,
                            "pair": pair_path.name, "camera": row["camera"]})
        if (frame - origin) / FPS > seconds:
            break
    pyboy.stop(save=False)
    report = {"rom": str(rom.relative_to(ROOT)), "rom_sha256": hashlib.sha256(rom.read_bytes()).hexdigest(),
              "emulator": "PyBoy 2.7.0", "fps": FPS, "music_start_frame": origin,
              "music_request_frame": origin - 4,
              "inputs": [], "cuts": cuts, "samples": samples,
              "frame_log": "frames.json", "all_frame_directory": str(frames_dir.relative_to(ROOT))}
    (output / "frames.json").write_text(json.dumps(rows, indent=2) + "\n")
    (output / "capture.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--seconds", type=float, default=28.5)
    args = parser.parse_args()
    capture(args.rom.resolve(), args.output.resolve(), args.seconds)

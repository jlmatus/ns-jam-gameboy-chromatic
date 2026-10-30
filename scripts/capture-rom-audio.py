"""Replay authorized controller intervals and capture genuine PyBoy APU PCM.

Usage: managed-python scripts/capture-rom-audio.py route.json output.wav
The JSON names the exact ROM, button sets/native-frame intervals, and optional
RGBA frame hashes from public emulator samples. No memory writes or patching.
"""
import hashlib
import json
from pathlib import Path
import sys
import wave

import numpy as np
from pyboy import PyBoy

route = json.loads(Path(sys.argv[1]).read_text())
output = Path(sys.argv[2])
if output.exists():
    raise SystemExit("Refusing to replace an existing audio capture")
rom = Path(route["romPath"])
identity = hashlib.sha256(rom.read_bytes()).hexdigest()
assert identity == route["romSha256"], "ROM identity changed"
p = PyBoy(str(rom), window="null", sound_emulated=True, sound_volume=100,
          sound_sample_rate=48000)
p.set_emulation_speed(0)
held = set()
buffers = []
checks = []
frame = 0
expected = {s["frame"]: s["rgbaSha256"] for s in route.get("samples", [])}
try:
    for action in route["actions"]:
        buttons = set(action["buttons"])
        for key in held - buttons:
            p.button_release(key)
        for key in buttons - held:
            p.button_press(key)
        held = buttons
        for _ in range(action["frames"]):
            p.tick(1, True, True)
            frame += 1
            buffers.append(p.sound.ndarray.copy())
            if frame in expected:
                digest = hashlib.sha256(p.screen.image.convert("RGBA").tobytes()).hexdigest()
                checks.append({"frame": frame, "expected": expected[frame], "actual": digest})
                assert digest == expected[frame], f"Replay diverged at frame {frame}"
finally:
    p.stop(save=False)
pcm8 = np.concatenate(buffers)
pcm16 = pcm8.astype(np.int16) * 256
with wave.open(str(output), "wb") as wav:
    wav.setnchannels(2)
    wav.setsampwidth(2)
    wav.setframerate(48000)
    wav.writeframes(pcm16.astype("<i2").tobytes())
receipt = {"romSha256": identity, "nativeFrames": frame, "sampleRate": 48000,
           "sampleFrames": len(pcm16), "channels": 2,
           "peak": int(np.abs(pcm16.astype(np.int32)).max()),
           "rms": float(np.sqrt(np.mean(pcm16.astype(np.float64) ** 2))),
           "nonzeroSamples": int(np.count_nonzero(pcm16)), "frameChecks": checks,
           "wavSha256": hashlib.sha256(output.read_bytes()).hexdigest()}
output.with_suffix(".json").write_text(json.dumps(receipt, indent=2) + "\n")
print(json.dumps(receipt))

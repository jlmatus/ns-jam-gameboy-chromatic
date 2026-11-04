"""Play the exact ROM in PyBoy: pick PLAYER as your player, drive to the right
rim, dunk, and keep every Dunk Cam frame as a GIF plus a contact strip.
Usage: play.py ROM PLAYER(0-5) OUTDIR [cpu]   ('cpu' waits for a CPU dunk instead)."""
import sys, json
import numpy as np
from pathlib import Path
from PIL import Image
from pyboy import PyBoy
rom, who, out = sys.argv[1], int(sys.argv[2]), Path(sys.argv[3]); out.mkdir(parents=True, exist_ok=True)
cpu = len(sys.argv) > 4 and sys.argv[4] == "cpu"
NAMES = ["MATUS", "MAGIC", "BAMBI", "CHRUX", "CHOMPA", "MEAT"]
JAM = 0xC1F6; ROSTER = 76; OWNER = 166; PASSER = 167; MODE = 168; SCORE = 176; TICK = 180; CTRL = 216
pb = PyBoy(rom, window="null", cgb=True); pb.set_emulation_speed(0)
n = 0; held = set(); log = []
def press(b):
    global held
    w = set(b)
    for x in held - w: pb.button_release(x)
    for x in w - held: pb.button_press(x)
    held = w
def step(b=(), k=1):
    global n
    press(b)
    for _ in range(k): pb.tick(); n += 1
m = lambda o: pb.memory[JAM + o]
def r16(a): v = pb.memory[a] | pb.memory[a + 1] << 8; return v - 65536 if v > 32767 else v
def pos(i): return r16(JAM + i * 19), r16(JAM + i * 19 + 2)
def ours(): return m(MODE) == 0 and m(OWNER) == m(CTRL) and m(OWNER) < 2
def score(): return r16(JAM + SCORE), r16(JAM + SCORE + 2)
# Boot, skip the intro, open selection, choose PLAYER, accept the rest.
step((), 600); step(["start"], 4); step((), 60); step(["a"], 4); step((), 60)
for _ in range(who): step(["right"], 2); step((), 20)
for _ in range(7): step(["a"], 4); step((), 40)
for _ in range(12):
    t = m(TICK); step((), 4)
    if m(TICK) != t: break
    step(["a"], 4); step((), 60)
roster = [m(ROSTER + i) for i in range(4)]
log.append({"frame": n, "roster": [NAMES[r] for r in roster]})
def capture():
    frames = []; before = score(); passer = m(PASSER) if m(PASSER) < 128 else m(PASSER) - 256
    log.append({"frame": n, "dunk_cam": True, "passer_slot": passer, "dunker": NAMES[roster[passer]], "score_before": before})
    while m(MODE) == 7:
        frames.append(Image.fromarray(np.asarray(pb.screen.ndarray)[:, :, :3].copy()))
        step((), 1)
    for _ in range(40): frames.append(Image.fromarray(np.asarray(pb.screen.ndarray)[:, :, :3].copy())); step((), 1)
    log.append({"frame": n, "score_after": score(), "frames": len(frames)})
    return frames, NAMES[roster[passer]], passer
def save(frames, tag):
    gif = frames[::2]
    gif[0].save(out / f"{tag}.gif", save_all=True, append_images=gif[1:], duration=33, loop=0, disposal=1)
    picks = [frames[i] for i in range(10, min(len(frames), 130), 12)][:10]
    strip = Image.new("RGB", (160 * len(picks), 144))
    for i, f in enumerate(picks): strip.paste(f, (160 * i, 0))
    strip.resize((strip.width * 2, 288), Image.NEAREST).save(out / f"{tag}-strip.png")
# Keep every attempt (makes, misses, blocks) until the chosen player scores.
limit = n + 60 * int(sys.argv[5] if len(sys.argv) > 5 else 300); made = False; attempts = 0
while n < limit and not made:
    if cpu:
        step([], 1)
        if m(MODE) == 7 and m(PASSER) >= 2:
            frames, name, slot = capture(); s = log[-1]["score_after"]
            made = s != log[-2]["score_before"]
            save(frames, f"{name.lower()}-left-{'made' if made else 'failed'}")
        continue
    if not ours():
        step(["a"] if m(MODE) == 6 and n % 20 < 2 else [], 1); continue
    if m(CTRL) != 0:
        step(["a"], 4); step((), 20); continue   # pass back to the chosen player
    x, y = pos(0); dirs = []
    if abs(x - 262 * 4) <= 32 and abs(y - 88 * 4) <= 48:
        step(["start", "b"], 4); step((), 1)
        if m(MODE) == 7:
            frames, name, slot = capture(); attempts += 1
            made = log[-1]["score_after"] != log[-2]["score_before"]
            save(frames, f"{name.lower()}-{'made' if made else 'failed-%d' % attempts}")
        continue
    if x < 262 * 4 - 8: dirs.append("right")
    if y < 88 * 4 - 16: dirs.append("down")
    elif y > 88 * 4 + 16: dirs.append("up")
    step(dirs, 1)
json.dump(log, open(out / f"run-{NAMES[who].lower()}{'-cpu' if cpu else ''}.json", "w"), indent=1)
print(json.dumps({"ok": made, "log": log}))

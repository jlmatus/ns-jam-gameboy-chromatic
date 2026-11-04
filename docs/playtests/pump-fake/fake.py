"""Play the ROM headless and exercise the pump fake / traveling rule.
Usage: python fake.py ROM LINEUP_INPUTS OUT_DIR [jam_symbol_hex]
Reads the live jam_game_t (packed SDCC layout) to know when the human holds
the ball, then: dribble toward the rim, fake until a defender bites, fake
again, shoot after a fake, and finally step with a dead ball (travel)."""
import sys
from pathlib import Path
from pyboy import PyBoy

rom, lineup, out = sys.argv[1], sys.argv[2], Path(sys.argv[3])
JAM = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0xC1F6
OFF = dict(owner=166, mode=168, height=169, charge=183, message=186, next_team=189,
           shots=202, controlled=216, picked_up=253, fake_time=254, bitten=255, bite_time=256)
MSG = {17: "FAKE", 18: "TRAVEL"}
out.mkdir(parents=True, exist_ok=True)
pb = PyBoy(rom, window="null", cgb=True, sound_emulated=False)
pb.set_emulation_speed(0)
held = set()
shots = 0
log = []

def press(buttons):
    global held
    want = set(buttons)
    for b in held - want: pb.button_release(b)
    for b in want - held: pb.button_press(b)
    held = want

def r8(name): return pb.memory[JAM + OFF[name]]
def r8_at(off): return pb.memory[JAM + off]
def s8(name):
    v = r8(name); return v - 256 if v > 127 else v
def r16(addr): return pb.memory[addr] | pb.memory[addr + 1] << 8
def player(i):
    base = JAM + i * 19
    x = r16(base); y = r16(base + 2)
    return (x - 65536 if x > 32767 else x), (y - 65536 if y > 32767 else y), pb.memory[base + 6]

def snap(label):
    global shots
    shots += 1
    pb.screen.image.convert("RGB").resize((640, 576), 0).save(out / f"{shots:02d}-{label}.png")

def state():
    return dict(mode=r8("mode"), owner=s8("owner"), ctl=r8("controlled"), pick=r8("picked_up"),
                fake=r8("fake_time"), bitten=r8("bitten"), msg=MSG.get(r8("message"), r8("message")),
                charge=r8("charge"), shots=r16(JAM + OFF["shots"]))

def run(buttons, frames):
    press(buttons); pb.tick(frames, True)

def ours():
    st = state(); return st["mode"] == 0 and st["owner"] == st["ctl"] and st["owner"] < 2

def get_ball(limit=4000):
    for t in range(limit):
        if ours(): return True
        st = state()
        # Inbound for our team: tap A; otherwise idle so the CPU plays out.
        run(["a"] if st["mode"] == 6 and t % 20 < 2 else [], 1)
    return False

def near_defender():
    hx, hy, _ = player(state()["ctl"])
    return min(abs(player(i)[0] - hx) + abs(player(i)[1] - hy) for i in (2, 3))

for line in Path(lineup).read_text().split("\n"):
    line = line.strip()
    if not line or line.startswith("#") or line.startswith("g"): continue
    label, buttons, frames = line.split()
    run([] if buttons == "-" else buttons.split(","), int(frames))

# Confirm remaining menus (length, review) until the match clock is set.
def live():
    before = pb.memory[JAM + 180]; pb.tick(4, True); return pb.memory[JAM + 180] != before
for t in range(12):
    if live(): break
    run(["a"], 4); run([], 60)
assert live(), "match did not start"
snap("tipoff")

def fake_cycle(tag):
    """Tap B for 3 frames, release, and record the fake (and any bite)."""
    run(["b"], 3); run([], 1)
    st = state(); log.append((tag, "release", st, near_defender()))
    snap(f"{tag}-fake")
    for k in range(3):
        run([], 3); st = state()
        bites = [i for i in (2, 3) if st["bitten"] >> i & 1]
        jumps = [player(i)[2] for i in (2, 3)]
        log.append((tag, f"+{3*(k+1)}", st, jumps))
        if k == 0: snap(f"{tag}-fake-bite{''.join(map(str,bites)) or 'none'}")
    return state()

attempt = 0
bit = False
while attempt < 12 and not (bit and attempt >= 3):
    assert get_ball(), "never gained the ball"
    # Dribble toward the right rim until a defender is close.
    for t in range(240):
        if not ours() or near_defender() < 110: break
        run(["right"], 1)
    if not ours(): continue
    run([], 2)
    attempt += 1
    st = fake_cycle(f"a{attempt}")
    bit = bool(st["bitten"] & 12) or any(e[2]["bitten"] & 12 for e in log[-4:])
    if st["mode"] == 0 and st["pick"]:
        if bit or attempt % 2 == 0:
            run([], 20); snap(f"a{attempt}-picked-up")
            # Real shot straight out of the fake.
            run(["b"], 30); snap(f"a{attempt}-gather"); run([], 4)
            st = state(); log.append((f"a{attempt}", "shot", st, None)); run([], 4); snap(f"a{attempt}-shot")
            run([], 30)
            log.append((f"a{attempt}", "contest", dict(block_attempts=r8_at(191), shot_blocked=r8_at(251),
                        blocks=[r16(JAM + 82 + 18*i + 14) for i in range(4)]), None))
        else:
            run([], 20)
            run(["right"], 4); st = state(); log.append((f"a{attempt}", "step", st, None)); snap(f"a{attempt}-travel"); run(["right"], 12); snap(f"a{attempt}-travel-hud")
            run([], 40); log.append((f"a{attempt}", "after", state(), None)); snap(f"a{attempt}-after-travel")

for entry in log: print(*entry)

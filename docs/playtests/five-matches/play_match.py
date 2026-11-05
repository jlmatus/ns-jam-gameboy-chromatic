"""Play one full NS JAM match on the exact ROM in PyBoy, as a human would with
the real buttons, and log every event, possession, stat and some screenshots.

Usage: play_match.py ROM OUTDIR SEED P0 P1 P2 P3 DIFFICULTY(0-2) [PERIODS]
P0 is you, P1 your CPU teammate, P2/P3 the opponents (0 MATUS .. 5 MEAT).
The bot only presses buttons; it reads RAM to decide, as a player reads the screen."""
import sys, json, random, math
import numpy as np
from pathlib import Path
from PIL import Image
from pyboy import PyBoy

rom, out, seed = sys.argv[1], Path(sys.argv[2]), int(sys.argv[3])
lineup = [int(v) for v in sys.argv[4:8]]; difficulty = int(sys.argv[8])
periods = int(sys.argv[9]) if len(sys.argv) > 9 else 4
out.mkdir(parents=True, exist_ok=True); (out / "shots").mkdir(exist_ok=True)
rng = random.Random(seed)
NAMES = ["MATUS", "MAGIC", "BAMBI", "CHRUX", "CHOMPA", "MEAT"]
RATINGS = [[7,10,10,8,2,3,5,9],[4,5,5,3,10,9,7,7],[4,5,6,4,4,7,10,7],[10,3,7,7,4,8,4,5],[5,8,5,3,7,7,9,6],[9,2,5,8,6,8,6,4]]
SPEED, THREE, PASS, STEAL, BLOCK, DUNK, POWER, CLUTCH = range(8)
MODES = ["HELD", "PASS", "SHOT", "LOOSE", "RESET", "FINISH", "INBOUND", "DUNK"]
MSGS = ["PLAY","PASS","STEAL","SHOT","REBOUND","TWO","THREE","MISS","BLOCK","CLOCK","REACH","SHOVE","RUNOVER","STAND","INBOUND","DUNK","OUT","FAKE","TRAVEL"]
JAM = 0xC1F6
RIM_R, RIM_L, RIM_Y = 280 * 4, 40 * 4, 88 * 4

pb = PyBoy(rom, window="null", cgb=True); pb.set_emulation_speed(0)
n = 0; held = set()
def press(b):
    global held
    w = set(b)
    for x in held - w: pb.button_release(x)
    for x in w - held: pb.button_press(x)
    held = w
def tick(b=(), k=1):
    global n
    press(b)
    for _ in range(k): pb.tick(1, False); n += 1
def shot(name):
    pb.tick(1, True); global n; n += 1
    Image.fromarray(np.asarray(pb.screen.ndarray)[:, :, :3].copy()).save(out / "shots" / f"{n:06d}-{name}.png")
u8 = lambda o: pb.memory[JAM + o]
def s8(o): v = u8(o); return v - 256 if v > 127 else v
def u16(o): return u8(o) | u8(o + 1) << 8
def s16(o): v = u16(o); return v - 65536 if v > 32767 else v
def P(i, f): return {"x": s16(i * 19), "y": s16(i * 19 + 2)}[f] if f in "xy" else u8(i * 19 + {"action":4,"cooldown":5,"block_time":6,"shot_time":7,"windup":9,"turbo":11,"stun":12}[f])
def state():
    return {"mode": u8(168), "owner": s8(166), "passer": s8(167), "ctrl": u8(216), "score": [u16(176), u16(178)],
            "msg": u8(186), "msg_time": u8(187), "clock": u16(243), "quarter": u8(249), "phase": u8(250),
            "ball": [s16(154), s16(156)], "height": u8(169), "paused": u8(181),
            "turbo": [u8(231 + i) for i in range(4)], "exhausted": [u8(239 + i) for i in range(4)],
            "pos": [[s16(i * 19), s16(i * 19 + 2)] for i in range(4)], "poss_time": u16(196)}
def stats():
    keys = ["points", "field_made", "field_attempted", "three_made", "three_attempted", "rebounds", "steals", "blocks", "dunks"]
    per = [{k: u16(82 + i * 18 + j * 2) for j, k in enumerate(keys)} for i in range(4)]
    tot = {k: u16(o) for k, o in [("passes",198),("steals",200),("shots",202),("rebounds",204),("baskets",206),("blocks",208),
           ("steal_attempts",210),("shoves",212),("runovers",214),("dunk_attempts",225),("dunks",227)]}
    return per, tot

# ---- Menu: boot, skip intro, choose the lineup, difficulty and length. ----
tick((), 600); tick(["start"], 4); tick((), 60); tick(["a"], 4); tick((), 60)
roster = [0, 1, 2, 3]; chosen = []
for stage in range(4):
    cur = roster[stage]
    while cur in chosen: cur = (cur + 1) % 6
    presses = 0
    while cur != lineup[stage]:
        cur = (cur + 1) % 6
        while cur in chosen: cur = (cur + 1) % 6
        presses += 1
    for _ in range(presses): tick(["right"], 2); tick((), 16)
    tick(["a"], 4); tick((), 40); chosen.append(lineup[stage])
for _ in range((difficulty - 1) % 3): tick(["right"], 2); tick((), 16)
tick(["a"], 4); tick((), 40)
for _ in range({1: 1, 2: 2, 4: 0}[periods]): tick(["right"], 2); tick((), 16)
tick(["a"], 4); tick((), 40); tick(["a"], 4); tick((), 40)
for _ in range(12):
    t = u8(180); tick((), 4)
    if u8(180) != t: break
    tick(["a"], 4); tick((), 60)
setup = [u8(76 + i) for i in range(6)]
assert setup[:4] == lineup and setup[4] == difficulty, (setup, lineup, difficulty)
shot("tipoff")

# ---- Player brain ----
events, possessions, samples = [], [], []
plan = None; plan_t = 0; last_owner = None; poss_start = n; poss_team = None
from collections import deque
history = deque(maxlen=10)  # a human reacts to what they saw ~8-10 frames ago
break_at = {}
import os
TRACE = [] if os.environ.get('JAM_TRACE') else None
DEBUG_FROM = int(os.environ.get('JAM_DEBUG_FROM', '0'))
BUZZER_B = bool(os.environ.get('JAM_BUZZER_B'))
# Builds with selective Dunk Cam expose jam.dunk_cam at offset 257; older builds show every dunk close up.
CAM_FLAG = bool(os.environ.get('JAM_CAM_FLAG'))
SHOT_AT = {int(v) for v in os.environ.get('JAM_SHOT_AT', '').split(',') if v}
cam = {"dunk_frames": 0, "cam_frames": 0, "dunks": [], "shots": []}
prev = state(); seen_msgs = {}; long_mode = {"mode": None, "since": n, "flagged": False}
fake_wait = 0; hold_b = 0; swipe_cd = 0; request_cd = 0
def name_of(i): return NAMES[lineup[i]] if 0 <= i < 4 else "-"
def dist(a, b): return math.hypot((a[0] - b[0]) / 4, (a[1] - b[1]) / 4)
def toward(me, tx, ty, slack=6):
    d = []
    if me[0] < tx - slack * 4: d.append("right")
    elif me[0] > tx + slack * 4: d.append("left")
    if me[1] < ty - slack * 4: d.append("down")
    elif me[1] > ty + slack * 4: d.append("up")
    return d
def choose_plan(c):
    r = RATINGS[lineup[c]]
    w = {"dunk": 1 + r[DUNK] * 1.2, "three": 1 + r[THREE] * 1.2, "mid": 3, "pass": 2}
    k = rng.choices(list(w), weights=list(w.values()))[0]
    return {"kind": k, "fake": rng.random() < 0.25, "spot_y": rng.choice([70, 88, 100, 112]) * 4,
            "deep": rng.choice([96, 100, 110, 125]) if k == "three" else 0}
def decide(s):
    global plan, plan_t, fake_wait, hold_b, swipe_cd, request_cd
    if BUZZER_B:  # regression check: press B into the buzzer and hold it into the break
        if s["phase"] == 0 and s["clock"] <= 1: return ["b"]
        if s["phase"] in (1, 2, 4) and n - break_at.get(s["quarter"] * 10 + s["phase"], n) < 60: return ["b"]
    if s["phase"] in (2, 4):  # quarter break / overtime card: read it, then press A
        return ["a"] if (n - break_at.get(s["quarter"] * 10 + s["phase"], n)) > 150 and n % 10 < 2 else []
    c = s["ctrl"]; me = s["pos"][c]; mode = s["mode"]; owner = s["owner"]
    turbo_ok = s["turbo"][c] > 30 and not s["exhausted"][c]
    if swipe_cd: swipe_cd -= 1
    if request_cd: request_cd -= 1
    if mode == 6:  # inbound: the inbounder passes in on A
        return ["a"] if n % 20 < 2 else []
    if mode == 0 and owner == c:  # we have the ball
        if plan is None: plan = choose_plan(c); plan_t = n
        defenders = [s["pos"][i] for i in (2, 3)]; close = min(dist(me, d) for d in defenders)
        if hold_b:
            hold_b -= 1; return ["b"] if hold_b else []
        if fake_wait:
            fake_wait -= 1
            if fake_wait == 0: hold_b = rng.randint(16, 38)
            return []
        r = RATINGS[lineup[c]]; dx = (RIM_R - me[0]) / 4
        if plan["kind"] == "pass" or (close < 12 and rng.random() < 0.02):
            plan = None; return ["a"]
        if n - plan_t > 600:  # possession dragging: just shoot
            hold_b = rng.randint(16, 38); return ["b"]
        if plan["kind"] == "dunk":
            rng_px = 10 + r[DUNK] * 4
            if 0 <= dx <= rng_px and abs(me[1] - RIM_Y) <= 16 * 4:
                return ["start", "b"] if s["turbo"][c] >= 12 else ["b"]
            d = toward(me, RIM_R - 20 * 4, RIM_Y, 3)
            return d + (["start"] if turbo_ok and dx > 30 else [])
        spot_x = RIM_R - (plan["deep"] if plan["kind"] == "three" else 60) * 4
        d = toward(me, spot_x, plan["spot_y"], 5)
        if d: return d + (["start"] if turbo_ok and abs(me[0] - spot_x) > 160 else [])
        if plan["fake"] and close < 30:
            plan["fake"] = False; fake_wait = 34; return ["b"]
        hold_b = rng.randint(16, 38); return ["b"]  # human timing: not always in the 18-36 window
    plan = None if owner != c else plan
    if mode == 0 and owner in (0, 1):  # teammate has it: get open, sometimes ask for it
        if request_cd == 0 and rng.random() < 0.01:
            request_cd = 120; return ["a"]
        return toward(me, RIM_R - 90 * 4, (70 if s["pos"][owner][1] > RIM_Y else 108) * 4, 6)
    if mode == 0 and owner >= 2:  # defend the carrier between him and our rim
        car = s["pos"][owner]; d = dist(me, car); acts = toward(me, car[0] - 14 * 4, car[1], 3)
        if turbo_ok and d > 40: acts.append("start")
        w = P(owner, "windup")
        if 3 <= w <= 14 and d < 40: return ["b"]
        if d < 18 and swipe_cd == 0:
            swipe_cd = rng.randint(25, 60)
            if rng.random() < 0.12 and s["turbo"][c] > 20: return ["start", "a"]
            return acts + ["a"]
        return acts
    if mode == 7 and s["passer"] >= 2:
        return ["b"] if n % 6 == 0 else []
    if mode == 2 and s["passer"] >= 2 and dist(me, s["pos"][s["passer"]]) < 30 and n % 4 == 0:
        return ["b"]
    if mode in (3, 1, 2):  # chase loose balls, passes and rebounds
        if mode == 1 and s["passer"] in (0, 1): return toward(me, RIM_R - 90 * 4, RIM_Y, 8)
        return toward(me, s["ball"][0], s["ball"][1], 2) + (["start"] if turbo_ok else [])
    return []

# ---- Event logging ----
def log_event(kind, s, extra=None):
    e = {"frame": n, "kind": kind, "q": s["quarter"], "clock": s["clock"], "score": s["score"],
         "owner": name_of(s["owner"]), "passer": name_of(s["passer"]), "ctrl": name_of(s["ctrl"]),
         "pos": s["pos"], "ball": s["ball"]}
    if extra: e.update(extra)
    events.append(e)
    if seen_msgs.get(kind, 0) < 2 and kind not in ("PASS", "PLAY"):
        seen_msgs[kind] = seen_msgs.get(kind, 0) + 1; shot(f"{kind.lower()}-{seen_msgs[kind]}")

limit = n + 60 * 60 * 40; final_seen = None
while n < limit:
    s = state()
    if s["phase"] == 3:  # final buzzer: stop pressing so the results screen stays up
        log_event("PHASE", s, {"phase": 3}); break
    # events from messages and state changes
    if s["msg_time"] > prev["msg_time"] + 1 or s["msg"] != prev["msg"]:
        log_event(MSGS[s["msg"]] if s["msg"] < len(MSGS) else f"MSG{s['msg']}", s)
    if s["score"] != prev["score"]:
        team = 0 if s["score"][0] != prev["score"][0] else 1
        log_event("SCORE", s, {"team": team, "points": (s["score"][team] - prev["score"][team]), "scorer": name_of(prev["passer"] if prev["passer"] >= 0 else s["passer"])})
    if s["quarter"] != prev["quarter"] or s["phase"] != prev["phase"]:
        break_at[s["quarter"] * 10 + s["phase"]] = n
        log_event("PHASE", s, {"phase": s["phase"]}); shot(f"phase{s['phase']}-q{s['quarter']}")
    if s["mode"] != prev["mode"]:
        if s["mode"] == 7: log_event("DUNK_CAM", s, {"dunker": name_of(s["passer"])})
        long_mode = {"mode": s["mode"], "since": n, "flagged": False}
    elif not long_mode["flagged"] and n - long_mode["since"] > {0: 1500, 3: 400, 6: 400, 4: 400}.get(s["mode"], 900) and not s["paused"]:
        long_mode["flagged"] = True; log_event("LONG_" + MODES[s["mode"]], s, {"frames": n - long_mode["since"]}); shot("long-" + MODES[s["mode"]].lower())
    if s["mode"] == 7:
        on = u8(257) if CAM_FLAG else 1
        cam["dunk_frames"] += 1; cam["cam_frames"] += 1 if on else 0
        if prev["mode"] != 7: cam["dunks"].append({"frame": n, "team": 0 if s["passer"] < 2 else 1, "cam": on, "cam_end": on})
        else: cam["dunks"][-1]["cam_end"] = on
    if s["mode"] == 2 and prev["mode"] != 2 and 0 <= s["passer"] < 4:
        x = s["pos"][s["passer"]][0]; d = abs((RIM_R if s["passer"] < 2 else RIM_L) - x) // 4
        cam["shots"].append({"frame": n, "who": name_of(s["passer"]), "team": 0 if s["passer"] < 2 else 1, "dist": d, "made": 0})
    if s["score"] != prev["score"] and cam["shots"] and n - cam["shots"][-1]["frame"] < 240:
        team = 0 if s["score"][0] != prev["score"][0] else 1
        if cam["shots"][-1]["team"] == team and prev["mode"] in (2, 5): cam["shots"][-1]["made"] = 1
    team_has = None if s["owner"] < 0 else (0 if s["owner"] < 2 else 1)
    if s["mode"] == 0 and team_has is not None and team_has != poss_team:
        if poss_team is not None: possessions.append({"team": poss_team, "start": poss_start, "end": n, "frames": n - poss_start})
        poss_team = team_has; poss_start = n
    if n % 30 == 0:
        samples.append({"frame": n, "mode": s["mode"], "owner": s["owner"], "ctrl": s["ctrl"], "pos": s["pos"], "ball": s["ball"],
                        "turbo": s["turbo"], "clock": s["clock"], "q": s["quarter"], "score": s["score"]})
    if n % 1800 == 0: shot("periodic")
    if n in SHOT_AT: shot("at")
    if TRACE is not None:  # per-frame camera and bodies, for jitter analysis
        TRACE.append([n, pb.memory[0xFF43], s["mode"], s["owner"], s["ctrl"], *[v for pos in s["pos"] for v in pos], *s["ball"],
                      *[P(i, "stun") for i in range(4)], *[pb.memory[JAM + i * 19 + 17] for i in range(4)], *[pb.memory[JAM + i * 19 + 18] for i in range(4)]])
    prev = s; history.append(s)
    seen = history[0] if not hold_b and not fake_wait else s
    if seen["ctrl"] != s["ctrl"] or s["phase"] != 0 or seen["phase"] != 0: seen = s  # never act on stale state across a break
    act = decide(seen)
    if DEBUG_FROM and DEBUG_FROM <= n <= DEBUG_FROM + 300: print("dbg", n, "phase", s["phase"], "q", s["quarter"], "mode", s["mode"], "press", act, "menu?", pb.memory[JAM + 250])
    tick(act, 1)

per, tot = stats(); s = state()
tick((), 240); shot("results")
for k in range(3): tick(["right"], 2); tick((), 60); shot(f"results-page{k+1}")
summary = {"lineup": [NAMES[i] for i in lineup], "difficulty": ["Warm-up", "Pickup", "Winners Stay"][difficulty],
           "periods": periods, "seed": seed, "frames": n, "final_score": s["score"], "phase": s["phase"], "quarter": s["quarter"],
           "players": {name_of(i): per[i] for i in range(4)}, "totals": tot, "dunk_cam": cam}
json.dump(summary, open(out / "summary.json", "w"), indent=1)
json.dump(events, open(out / "events.json", "w"))
json.dump(possessions, open(out / "possessions.json", "w"))
json.dump(samples, open(out / "samples.json", "w"))
if TRACE is not None: np.save(os.environ['JAM_TRACE'], np.array(TRACE, dtype=np.int32))
print(json.dumps({k: summary[k] for k in ("lineup", "difficulty", "final_score", "frames", "phase")}))

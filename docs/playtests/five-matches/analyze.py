"""Summarize the five recorded matches: box scores, how possessions end, shot
distances, time spent in each game mode, and anything that stalled."""
import json, math, collections, sys
from pathlib import Path
D = Path(__file__).parent
MODES = ["HELD","PASS","SHOT","LOOSE","RESET","FINISH","INBOUND","DUNK"]
for g in sorted(D.glob("g?")):
    S = json.load(open(g/"summary.json")); E = json.load(open(g/"events.json")); P = json.load(open(g/"possessions.json")); X = json.load(open(g/"samples.json"))
    L = S["lineup"]; team = lambda name: 0 if name in L[:2] else 1
    print(f"\n=== {g.name}: {L[0]}(you)+{L[1]} vs {L[2]}+{L[3]}  {S['difficulty']}  final {S['final_score']}  frames {S['frames']}")
    for name, st in S["players"].items():
        fg = f"{st['field_made']}/{st['field_attempted']}"; tp = f"{st['three_made']}/{st['three_attempted']}"
        print(f"  {name:7s} pts {st['points']:3d} fg {fg:7s} 3pt {tp:6s} reb {st['rebounds']:2d} stl {st['steals']:2d} blk {st['blocks']:2d} dnk {st['dunks']:2d}")
    print("  totals", S["totals"])
    # shot distances and results by team
    shots = [e for e in E if e["kind"] == "SHOT"]
    for t in (0, 1):
        ds = []
        for e in shots:
            if e["passer"] == "-" or team(e["passer"]) != t: continue
            i = L.index(e["passer"]); x, y = e["pos"][i]; rim = 280*4 if t == 0 else 40*4
            ds.append(round(abs(x - rim) / 4))
        ds.sort(); print(f"  team{t} jump-shot distances px: n={len(ds)} median={ds[len(ds)//2] if ds else '-'} >150px={sum(d>150 for d in ds)} list={ds[:40]}")
    # possessions
    for t in (0, 1):
        pp = [p["frames"] for p in P if p["team"] == t]
        if pp: pp.sort(); print(f"  team{t} possessions n={len(pp)} median {pp[len(pp)//2]/60:.1f}s max {pp[-1]/60:.1f}s")
    # what ends opponent possessions: next event after each possession start
    k = collections.Counter(e["kind"] for e in E if e["kind"] not in ("PLAY","PASS"))
    print("  events", dict(k.most_common()))
    who = collections.Counter((e["kind"], e["owner"] if e["kind"] in ("STEAL","REACH","TRAVEL","OUT") else e["passer"]) for e in E if e["kind"] in ("STEAL","BLOCK","TRAVEL","OUT","RUNOVER","SHOVE","FAKE","DUNK_CAM","REACH","STAND"))
    print("  by player", dict(who.most_common(30)))
    mt = collections.Counter(MODES[s["mode"]] for s in X); tot = sum(mt.values())
    print("  time in mode", {m: f"{100*c/tot:.0f}%" for m, c in mt.most_common()})
    print("  stalls", [(e["kind"], e["frame"], e.get("frames"), e["owner"]) for e in E if e["kind"].startswith("LONG")])

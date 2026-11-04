"""How each team's possessions end, and the longest opponent possession."""
import json, collections
from pathlib import Path
D = Path(__file__).parent
END = ("STEAL", "SHOT", "DUNK_CAM", "TRAVEL", "OUT", "REACH", "SHOVE", "RUNOVER")
for g in sorted(D.glob("g?")):
    S = json.load(open(g/"summary.json")); E = json.load(open(g/"events.json")); P = json.load(open(g/"possessions.json"))
    L = S["lineup"]
    for t in (0, 1):
        c = collections.Counter()
        for p in P:
            if p["team"] != t: continue
            ev = [e for e in E if p["start"] <= e["frame"] <= p["end"] and e["kind"] in END]
            # last relevant event before the ball changed hands
            c[ev[-1]["kind"] if ev else "none"] += 1
        print(g.name, "team", t, L[2*t:2*t+2], dict(c.most_common()))
    P1 = sorted([p for p in P if p["team"] == 1], key=lambda p: -p["frames"])[0]
    ev = [(e["frame"], e["kind"], e["owner"], e["clock"]) for e in E if P1["start"] <= e["frame"] <= P1["end"]]
    print("   longest opp possession", P1["frames"], "frames:", ev[:12])
    tot = S["totals"]; print("   steal attempts", tot["steal_attempts"], "steals", tot["steals"], "| dunks", tot["dunks"], "/", tot["dunk_attempts"])

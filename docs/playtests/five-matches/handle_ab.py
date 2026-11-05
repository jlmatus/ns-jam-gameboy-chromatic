"""HANDLE/REBOUND A/B: per-player points, rebounds, steals and field goals.

Usage: python3 handle_ab.py <runs-dir>
<runs-dir> holds one folder per match named <build>-<lineup>-<seed>, each with
the summary.json that play_match.py writes (handle-ab/ keeps only those).
"""
import json, os, sys
from collections import defaultdict

def main(root):
    per = defaultdict(lambda: defaultdict(lambda: defaultdict(int)))
    tot = defaultdict(lambda: defaultdict(int))
    lineups = {}
    for name in sorted(os.listdir(root)):
        path = os.path.join(root, name, "summary.json")
        if not os.path.isfile(path):
            continue
        build, lineup, _ = name.split("-")
        d = json.load(open(path))
        lineups[lineup] = (d["lineup"], d["difficulty"])
        for who, p in d["players"].items():
            for k in ("points", "rebounds", "steals", "field_made", "field_attempted"):
                per[(lineup, who)][build][k] += p[k]
        for k in ("steals", "rebounds", "baskets", "runovers"):
            tot[build][k] += d["totals"][k]
        tot[build]["our"] += d["final_score"][0]; tot[build]["cpu"] += d["final_score"][1]
        tot[build]["wins"] += d["final_score"][0] > d["final_score"][1]
    print("| # | Lineup (you, CPU teammate vs two CPUs), level | Player | Points | Rebounds | Steals | Field goals |")
    print("|---|---|---|---|---|---|---|")
    for lineup in sorted(lineups):
        names, level = lineups[lineup]
        for i, who in enumerate(names):
            o, n = per[(lineup, who)]["old"], per[(lineup, who)]["new"]
            head = f"| {lineup[1]} | {', '.join(names[:2])} vs {', '.join(names[2:])}, {level} |" if i == 0 else "| | |"
            print(f"{head} {who} | {o['points']} → {n['points']} | {o['rebounds']} → {n['rebounds']} | {o['steals']} → {n['steals']} | "
                  f"{o['field_made']}/{o['field_attempted']} → {n['field_made']}/{n['field_attempted']} |")
    for b in ("old", "new"):
        t = tot[b]
        print(f"\n{b}: your side {t['our']} points, CPU {t['cpu']}, your wins {t['wins']}; steals {t['steals']}, rebounds {t['rebounds']}, baskets {t['baskets']}, runovers {t['runovers']}")

if __name__ == "__main__":
    main(sys.argv[1])

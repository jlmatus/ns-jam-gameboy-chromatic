"""Off-ball A/B: spacing, pile-ups, cuts and coverage from match samples.

Usage: python3 offball_ab.py <runs-dir>
<runs-dir> holds one folder per match named <build>-<lineup>-<seed> (for example
old-g1-101), each with the summary.json, samples.json and events.json that
play_match.py writes. Samples are taken every 30 frames; positions are quarter
pixels. Only the summaries are kept in offball-ab/ (samples and events total
about 15 MB); offball-ab/table.md is this script's output for that run.
"""
import json, os, sys, statistics
from collections import defaultdict

RIM_X = (280 * 4, 40 * 4)   # rim each team attacks
RIM_Y = 88 * 4

def near(a, b):
    return (abs(a[0] - b[0]) + abs(a[1] - b[1])) // 4

def match_metrics(folder):
    samples = json.load(open(os.path.join(folder, "samples.json")))
    summary = json.load(open(os.path.join(folder, "summary.json")))
    held = [s for s in samples if s["mode"] == 0 and s["owner"] >= 0]
    spacing, covered, piles, at_rim = [], [], 0, 0
    for s in held:
        o = s["owner"]; team = o >> 1; mate = o ^ 1; pos = s["pos"]
        spacing.append(near(pos[o], pos[mate]))
        covered.append(min(near(pos[mate], pos[d]) for d in range(4) if d >> 1 != team))
        piles += sum(1 for i in range(4) if i != o and near(pos[i], pos[o]) < 16) >= 2
        at_rim += abs(pos[mate][0] - RIM_X[team]) < 32 * 4 and abs(pos[mate][1] - RIM_Y) < 20 * 4
    # Feeds: passes caught by a teammate within 32 px of the rim; scored if
    # that team scores within the next 150 frames (about 2.5 seconds).
    events = json.load(open(os.path.join(folder, "events.json")))
    feeds = scored = 0
    for i, e in enumerate(events):
        if e["kind"] != "PASS" or e["passer"] in (None, "-"):
            continue
        passer = next((j for j in range(4) if summary["lineup"][j] == e["passer"]), None)
        if passer is None:
            continue
        mate = passer ^ 1; team = passer >> 1
        if abs(e["pos"][mate][0] - RIM_X[team]) < 32 * 4 and abs(e["pos"][mate][1] - RIM_Y) < 24 * 4:
            feeds += 1
            scored += any(f["kind"] == "SCORE" and f.get("team") == team and 0 < f["frame"] - e["frame"] < 150 for f in events[i:i + 12])
    return {"feeds": feeds, "feed_scores": scored, "held": len(held), "spacing": spacing, "covered": covered, "piles": piles, "at_rim": at_rim,
            "passes": summary["totals"]["passes"], "score": summary["final_score"],
            "steals": summary["totals"]["steals"]}

def main(root):
    agg = defaultdict(lambda: defaultdict(list))
    for name in sorted(os.listdir(root)):
        path = os.path.join(root, name)
        if not os.path.isfile(os.path.join(path, "samples.json")):
            continue
        build, lineup, seed = name.split("-")
        m = match_metrics(path)
        for k, v in m.items():
            agg[(lineup, build)][k].append(v)
    print("| # | Build | Teammate spacing (median px) | Off-ball man's nearest defender (median px) | Pile-ups (% of held samples) | Teammate at the rim (%) | Passes | Feeds to the rim (scored) | Steals | Scores |")
    print("|---|---|---|---|---|---|---|---|---|---|")
    for lineup in sorted({k[0] for k in agg}):
        for build in ("old", "new"):
            a = agg.get((lineup, build))
            if not a:
                continue
            held = sum(a["held"])
            spacing = statistics.median([v for m in a["spacing"] for v in m])
            covered = statistics.median([v for m in a["covered"] for v in m])
            scores = ", ".join(f"{s[0]}–{s[1]}" for s in a["score"])
            print(f"| {lineup[1]} | {build} | {spacing:.0f} | {covered:.0f} | {100 * sum(a['piles']) / held:.1f} | "
                  f"{100 * sum(a['at_rim']) / held:.1f} | {sum(a['passes'])} | {sum(a['feeds'])} ({sum(a['feed_scores'])}) | {sum(a['steals'])} | {scores} |")

if __name__ == "__main__":
    main(sys.argv[1])

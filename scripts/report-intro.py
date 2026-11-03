#!/usr/bin/env python3
"""Package real-ROM intro evidence, without modifying any game asset."""
import hashlib
import html
import json
import math
import subprocess
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs/playtests/intro"
read = lambda name: json.loads((OUT / name).read_text())
capture, frames, actions, build = [read(n) for n in ("capture.json", "frames.json", "actions.json", "build.json")]
timeline = json.loads((ROOT / "intro/timeline.json").read_text())
validation = json.loads((ROOT / "intro/validation.json").read_text())
manifest = json.loads((ROOT / "game/build/intro-assembly.json").read_text())
fps = capture["fps"]

# Verify imported input bytes, including all manual-color sidecars and palettes.
input_integrity = all(hashlib.sha256((ROOT / p).read_bytes()).hexdigest() == sha
                      for p, sha in manifest["source_sha256"].items())
manual_color = all(json.loads((ROOT / f"game/assets/backgrounds/{s['background']}.png.gbsres").read_text())["autoColor"] is False
                   for s in timeline["shots"])

# The stationary Matus ball gives a precise anchor check independent of shake,
# video compression, or motion/frame rounding.
source = Image.open(ROOT / "intro/gbstudio/assets/sprites/intro_ball.png").convert("RGB")
native = Image.open(OUT / "rom-9_6.png").convert("RGB")
sprite_colors = {"E0F8CF": 1, "86C06C": 2, "071821": 3}
duotone = timeline["shots"][3]["script"]
duotone_id = next(s["slots"]["0"] for s in duotone if s["op"] == "set_sprite_palettes" and s["set"] == "duotone")
palette = next(p["colors"] for p in timeline["palettes"] if p["id"] == duotone_id)
matched = tested = 0
for y in range(32):
    for x in range(32):
        pixel = source.getpixel((x, y))
        if pixel == (101, 255, 0):
            continue
        key = "".join(f"{v:02X}" for v in pixel)
        color = palette[sprite_colors[key]]
        expected = tuple((int(color[k:k + 2], 16) >> 3) << 3 for k in (0, 2, 4))
        actual = native.getpixel((84 + x, y))
        tested += 1
        matched += actual == expected

white_ranges = []
start = previous = None
for row in frames:
    rgb = row["uniform_rgb"]
    white = rgb is not None and min(rgb) >= 240 and row["intro_frame"] >= 0
    if white and start is None:
        start = row
    if start is not None and not white:
        white_ranges.append({"scene": start["scene"], "start_s": start["time_s"],
                             "end_s": previous["time_s"], "frames": previous["frame"] - start["frame"] + 1})
        start = None
    previous = row

rom_banks = [b for b in read("bank-usage.json")["banks"] if b["name"].startswith("ROM_")]
bank_usage = {"declared_banks": build["rom"]["sizeBytes"] // 16384,
              "nonempty_banks": len(rom_banks),
              "highest_bank": max(int(b["name"].split("_")[1]) for b in rom_banks),
              "used_bytes": sum(int(b["used"]) for b in rom_banks)}
tile_counts = [{"scene": c["scene"], "expected": validation[c["scene"]]["unique_tiles"],
                "reported": c["background"]["uniqueTiles"],
                "with_flip_deduplication": c["background"]["effectiveUniqueTiles"],
                "palette_slots": c["palettes"]["background"]["resolvedSlotCount"]}
               for c in build["counts"]]
fade_scenes = sorted(set(r["scene"] for r in frames if r["fade_running"]))
checks = [
    {"item": "No added text in the intro", "pass": True,
     "evidence": "All 14 native scene scripts and the 15 screenshot pairs; supplied jersey numbers remain in the supplied art."},
    {"item": "No unplanned white/black frames", "pass": False,
     "evidence": "Some incoming scene loads extend designed flashes. At 25.1s the native frame remains white while the reference already shows the rollcall. See pair-25_1.png and frames.json."},
    {"item": "Fades only after shots 6 and 14", "pass": fade_scenes == ["intro_06_magic_block", "intro_14_rollcall_5"],
     "evidence": "Per-frame fade_running values in frames.json. Nonmodal native fade steps remain skippable."},
    {"item": "Cuts match the reference/music exactly", "pass": False,
     "evidence": "Music routine 0 drives all cut requests and per-scene VBlank timestamps. Visible scene loading is longer than the simulator reference; strict frame matching therefore fails."},
    {"item": "Actor offset calibrated at Matus's fingertips", "pass": matched == tested,
     "evidence": f"Set Position uses timeline + (0,8) pixels. Stationary ball: {matched}/{tested} original opaque pixels match native 5-bit palette output at the expected coordinates. See pair-9_6.png."},
    {"item": "A and START skip to title", "pass": actions["all_skips_pass"],
     "evidence": f"{sum(t['pass'] for t in actions['skip_tests'])}/{len(actions['skip_tests'])} real-ROM input tests, including two-frame taps during the glass shot and both fades. See actions.json and skip screenshots. Earlier failed short taps are retained under earlier-short-tap/."},
    {"item": "Intro ends on title; replay and Play Game work", "pass": actions["replay_intro_pass"] and actions["title_to_selection_pass"] and frames[-1]["scene"] == "managua_court" and frames[-1]["music_bank"] == 255,
     "evidence": "capture.json, title.png, replay.png, selection.png and actions.json."},
    {"item": "Supplied assets unchanged; manual color remains off", "pass": input_integrity and manual_color,
     "evidence": f"SHA-256 equality for all {len(manifest['source_sha256'])} imported files; all 14 autoColor fields are false."},
    {"item": "Tile/palette counts match validation", "pass": all(c["expected"] == c["reported"] and c["palette_slots"] == 8 for c in tile_counts),
     "evidence": "ModRetro graphics analysis in build.json. Compiled flip deduplication reduces allocation without altering PNGs."},
]
audit = {"rom_sha256": capture["rom_sha256"], "checklist": checks, "actor_anchor_px": [0, 8],
         "actor_opaque_pixel_matches": [matched, tested], "white_intervals": white_ranges,
         "tile_counts": tile_counts, "rom_bank_usage": bank_usage,
         "definition_of_done_met": all(c["pass"] for c in checks),
         "limitations": ["GB Studio scene-load flash durations exceed the simulator reference in some shots.",
                         "Camera shake uses the native engine RNG; the reference simulator can choose a different displacement.",
                         "Video reference is compressed and native RGB output expands 5-bit channels differently.",
                         "No physical cartridge was flashed or tested."]}
(OUT / "audit.json").write_text(json.dumps(audit, indent=2) + "\n")

for title, samples, columns in [("pairs-shots-1-4", capture["samples"][:6], 2),
                                ("pairs-shots-5-14", capture["samples"][6:], 3)]:
    sheet = Image.new("RGB", (644 * columns, 305 * math.ceil(len(samples) / columns)), "#18232e")
    for index, sample in enumerate(samples):
        pair = Image.open(OUT / sample["pair"]).convert("RGB").resize((644, 305), Image.Resampling.LANCZOS)
        sheet.paste(pair, ((index % columns) * 644, (index // columns) * 305))
    sheet.save(OUT / f"{title}.png")

rows = "".join(f"<tr><td>{html.escape(c['item'])}</td><td class={'good' if c['pass'] else 'bad'}>{'PASS' if c['pass'] else 'FAIL'}</td><td>{html.escape(c['evidence'])}</td></tr>" for c in checks)
tiles = "".join(f"<tr><td>{c['scene']}</td><td>{c['expected']}</td><td>{c['reported']}</td><td>{c['with_flip_deduplication']}</td><td>{c['palette_slots']}</td></tr>" for c in tile_counts)
pairs = "".join(f"<figure><a href='{s['pair']}'><img loading='lazy' src='{s['pair']}' alt='Reference and ROM at {s['time_s']} seconds'></a><figcaption>{s['time_s']}s · {s['scene']} · native frame {s['frame']}</figcaption></figure>" for s in capture["samples"])
page = f"""<!doctype html><html lang=en><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>
<title>NS JAM native intro verification</title><style>
body{{font:16px/1.5 system-ui;margin:32px auto;max-width:1288px;padding:0 20px;background:#0b1825;color:#e9edf0}}
h1,h2{{line-height:1.2}}a{{color:#8dd6df}}table{{border-collapse:collapse;width:100%;margin:20px 0}}td,th{{padding:10px;border:1px solid #385260;text-align:left}}.good{{color:#9eddab}}.bad{{color:#ffbb84}}img,video{{max-width:100%;height:auto}}figure{{margin:28px 0}}figcaption{{color:#b8c5cc}}code{{overflow-wrap:anywhere}}
</style><h1>NS JAM — complete native intro review build</h1>
<p>All fourteen editable GB Studio scenes are assembled. Boot, music cues, skip, final title, replay and title-to-selection are implemented. Strict visual acceptance remains open because native scene loads extend some designed flashes. Supplied images, palettes and music are unchanged.</p>
<video controls preload=metadata src='native-intro.mp4' poster='rom-9_6.png'></video>
<p>Exact ROM: <code>{capture['rom_sha256']}</code>. Color Only, 4 MiB. {bank_usage['nonempty_banks']} nonempty banks; highest bank {bank_usage['highest_bank']}; {bank_usage['used_bytes']:,} allocated ROM bytes.</p>
<h2>Checklist</h2><table><tr><th>Item</th><th>Result</th><th>Evidence / limitation</th></tr>{rows}</table>
<p><a href='audit.json'>Audit</a> · <a href='capture.json'>Captures and cuts</a> · <a href='frames.json'>Every-frame log</a> · <a href='actions.json'>Controller tests</a> · <a href='build.json'>Build and graphics analysis</a> · <a href='bank-usage.json'>GBDK ROM usage</a></p>
<h2>Tiles and palettes</h2><table><tr><th>Scene</th><th>validation.json</th><th>Reported unique tiles</th><th>Compiled with flips</th><th>Palette slots</th></tr>{tiles}</table>
<h2>All requested screenshot pairs</h2><p>Reference on the left; exact-ROM screenshot on the right. Click a pair for its original dimensions.</p>{pairs}
<h2>Title and replay</h2><img src=title.png alt='Night Court title'><img src=replay.png alt='Replay intro'><img src=selection.png alt='Play Game selection'>
<p>Host simulation tests and native sprite-loader budget passed (190/192 court sprite tiles). See oam-sample.json for the exact-build glass-shot hardware sample. No physical hardware acceptance is claimed.</p></html>"""
(OUT / "index.html").write_text(page)
print(json.dumps({"anchor_matches": [matched, tested], "checks": [(c["item"], c["pass"]) for c in checks], "rom_banks": bank_usage}))

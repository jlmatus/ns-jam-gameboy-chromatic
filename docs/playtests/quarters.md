# Four-quarter verification

## Checks

- Sanitized host tests pass with warnings as errors. New tests cover exact 180-second periods (10,754 native VBlanks), chunk-independent timing, paused/dead-ball/Dunk Cam freezes, score/lineup/stat preservation, alternating quarter inbounds, no late shots, made/missed buzzer shots, Q4 final freeze and same-lineup replay. Existing shooting, contact, selection, inbounds and turbo tests also pass: [host output](quarters/host-tests.txt).
- Official native ROM SHA-256 `784f1cd716c3ca35e5a9f3540d74f3708377d37045a4ad93043360efdc996c81` builds and passes cartridge-header inspection as a 2 MiB GBC-only image. Only previously classified CLI deprecation/SDCC optimizer warnings remain.
- Genuine PyBoy 2.7.0 playback starts Q1 at 3:00. From frame 289 to 409, exactly 120 VBlanks decrement 180 to 178 seconds, including the correct fractional carry. Clock seconds/fraction are unchanged across frames 421–1021 while SELECT-paused.
- A complete native match reaches Q1/Q2/Q3 breaks and Q4 final. Retained samples and state reads from the continuing recording show quarter ends at frames 14,688 / 29,112 / 43,536 / 57,960, with scores 0:2 / 0:2 / 0:4 / 0:6. These are sampled observations after the boundary, not exact buzzer frames. A advances each break, preserving scores and resetting the next clock to 3:00.
- The final state remains byte-identical through another 600 native frames. A replays the same lineup at Q1 3:00 with scores reset. A subsequent MATUS dunk preserves both clock seconds (175) and fraction (10,196) across frames 58,880–58,930; its HUD still shows Q1, 2:55 and turbo.
- Official browser export ROM SHA-256 `98a4ab0d0e22e18139179a30ba53885e671ea06f3b6b563a2886c1dd15ce445f` is verified by the preview's export identity. Its clock/HUD was played in the existing built-in-browser tab; the previous game state was retained before replacement. The preview is left SELECT-paused; P resumes.
- Official source export and sprite-budget check pass at 190/192 reserved sprite tiles. Clock/quarter text uses the existing font; it adds no tiles or sprites. `git diff --check` passes.

## Evidence and limits

[Q1 HUD](quarters/q1-start.png), [quarter break](quarters/q1-end.png), [Q2 inbound](quarters/q2-inbound.png), [Q4 final](quarters/q4-final.png), [clock in Dunk Cam](quarters/dunk-clock.png), [browser](quarters/browser-clock.png), and [receipts/input intervals](quarters-evidence.json).

The full-match recording retains periodic genuine frames (every 60 VBlanks), selected call endpoints and complete bounded input history; gaps are not interpolated. Host tests isolate buzzer outcomes; the full native match exercises ordinary gameplay timing. Native and browser exports have separate binary identities. No physical Chromatic test was performed. Overtime, richer intermissions/results, sound and saved records remain pending.

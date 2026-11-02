# NS JAM native front-end proof

## Delivered

The project/ROM now display NS JAM. Cold boot plays a twelve-pose MATUS shot, then opens Night Court. Home offers Play Game, How to Play and Replay Intro. MATUS's main card uses a 96×96 whole-cell portrait and separate rating column; existing rating pages and all other cards remain available. This proof precedes the full five-player cinematic and remaining card redesigns.

## Validation

- `sh scripts/test.sh` passes with sanitizers/warnings as errors: front timing/chunk invariance, skipping at every elapsed VBlank, held-input release guards, menu wrap, help/back, replay and START-to-play, plus the existing simulation, stats/results, period/OT, contact, shooting and turbo checks. [Host output](front-proof/host-tests.txt).
- Official CLI native build exits zero; exact ROM SHA-256 `daab3f0e4c3138e5e64ba003d81a1e1fb2143e3100c63bfbadeb2bff4619b38a`. Valid GBC-only 4 MiB MBC5 cartridge, header title `NSJAM`, matching debug artifacts. Existing SDCC optimizer and stock SFX inline-helper warnings remain; no errors.
- Genuine PyBoy 2.7.0 play records boot animation through home, MATUS's card, unchanged all-ratings page, held-A protection, MATUS as CPU teammate, lineup review and fresh 3:00 match entry. Court, sprites, ball and ordinary made-basket/inbound rendering restore correctly. SELECT pause/B returns to selection; B returns home; help, replay, held replay-A and START skip are verified. No memory writes or shortened game clocks.
- Twenty native 160×144 images pass the four-colors-per-8×8-tile check and use seven BG palettes. Static pages fit 166–227 patterns. Intro common art plus two dynamic regions use 240/256 patterns at signed BG indices 0–127 in both banks. The font/gauge range and OBJ patterns remain separate. Official source export plus sprite check passes at 190/192; `git diff --check` passes.
- Early recordings retain the boot crash after adding RAM buffers and partial visible uploads. The integrated version reuses inactive menu/palette RAM, swaps hidden maps with double-buffered pose patterns and stages page transitions in navy. Native sampled frames and the retained clip show the repaired animation and dark transition; no LCD-off white frame is used.
- Official browser export SHA-256 `eaca4a09dc080e9c06dcb6d6cefd33a133bfddbe75c1ae8bd18f9b9d6f9d1c41`. Built-in browser shows Night Court, opens the new MATUS card, returns home with B, and plays Replay Intro. It remains open for review. Native-app inventory reported a locked Mac, but the built-in browser controls remained available. No physical Chromatic test or audible-quality measurement was performed.

## Evidence

[Actual native animation](front-proof/matus-intro.gif), [home](front-proof/home.png), [MATUS card](front-proof/matus-card.png), [rating page](front-proof/ratings.png), [MATUS teammate header](front-proof/matus-teammate.png), [match entry](front-proof/match-entry.png), [ordinary made basket](front-proof/live-basket.png), [help](front-proof/help.png), [replay](front-proof/replay.png), [skip](front-proof/skip-home.png), [browser home](front-proof/browser-home.png), [browser card](front-proof/browser-card.png), and [build/input/frame receipts](front-proof-evidence.json).

Final retained recording: `game/build/front-delivery`, session `906bc2e51c8148cea258e986c8e59d42`, branch `branch-0001`, finalized passed at frame 1,718. It samples every VBlank and retains the controller journal. The GIF uses actual frames 110–440: 331 source frames condensed to 14 distinct held images over 5,540 ms, including the transition/home hold. No animation frames are invented. Earlier review/failure recordings were reversibly archived to release plugin quota.

The proof has one MATUS spotlight, temporary kit art and short existing shot/swish cues. The other four intro spotlights, full theme, remaining card redesigns, broader native Dunk Cam regression and physical-device validation remain future work.

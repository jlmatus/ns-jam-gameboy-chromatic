# Final results verification

## Implemented

A full-screen final summary identifies both lineups, final score, winning pair and overtime count. Left/right wraps between the summary and four player box-score pages. A rematches with the same setup; B changes the lineup. Both actions remain visible on every page, and held input cannot skip the result on entry. Player totals are match-only; saved records remain deferred. See [stat definitions](../design/results.md).

## Validation

- Sanitizer/warnings-as-errors host suite passes. New checks cover scoring for all four slots, two/three-point makes and attempts, made dunks, exact-once baskets, rebound ownership, exclusion of fumble pickups, 512 two-defender shot contests with at most one block, and a 30,000-update match where all per-player totals reconcile with team/aggregate counters after every update. Period/OT retention, rematch clearing, all results pages, both winners, bounded tile output, navigation wrap and held-input guards are tested. [Host output](final-results/host-tests.txt).
- Official native build exits zero. Exact ROM SHA-256: `a93941192c3e547dece55628751f9277cd2f514bd124ec28f9ec1a1d08ab3b62`. Valid GBC-only 4 MiB MBC5 cartridge with matching debug artifacts. Existing SDCC optimizer and stock SFX helper warnings remain; no errors.
- Genuine PyBoy 2.7.0 input-driven play uses one full three-minute Warm-up period and three one-minute overtimes. MATUS/MAGIC beat BAMBI/TORRES 4–2. No memory writes or shortened clock. Final summary at frame 28,192 shows both winning names and OT 3. Player pages at 28,200 / 28,216 / 28,232 / 28,248 show MATUS 2 points, MAGIC 2, BAMBI 0, TORRES 2. MAGIC's page credits the observed blocked BAMBI dunk. All pages fit the native 160×144 display with readable actions.
- A from the fourth stat page restarts Q1 3:00, 0–0, with intact court rendering. A neutral same-ROM checkpoint at frame 28,256 allows a separate B branch to verify return to MATUS's selection card. Another branch verifies page-four-to-summary wrap. Recorded controls, sample hashes and branches are retained in [receipts](final-results-evidence.json).
- Official source export and sprite allocation check pass at 190/192 tiles. Results reuse the existing font and menu palette without additional image/OAM allocations. `git diff --check` passes.
- Official browser export SHA-256: `4aa38132cbf0a2f5a2388c755b9e5b8ab7b094da2e7f3e79af5250377a73dc58`. The previous paused game was captured before replacing the build. The built-in browser is open on the new build's player-selection screen; status confirms the new ROM identity. Full results/navigation are verified in native PyBoy, rather than claimed from browser playback. Physical Chromatic is untested.

## Captures

[Summary](final-results/summary.png), [MATUS](final-results/matus.png), [MAGIC](final-results/magic.png), [BAMBI](final-results/bambi.png), [TORRES](final-results/torres.png), [rematch](final-results/rematch.png), [change lineup](final-results/change-lineup.png), and [summary wrap](final-results/summary-wrap.png).

Retained recording: `game/build/final-results`, session `e568a7f54c3f4f958c33b8f43b6e33f7`, branches `branch-0001` through `branch-0003`, finalized passed. Periodic genuine frames are sampled every 60 VBlanks plus call endpoints; the complete input journal is retained. Samples establish observed states, not all intervening animation frames or audible sound quality.

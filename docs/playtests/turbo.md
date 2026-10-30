# Turbo system verification

## Implemented behavior

The top-right four-cell gauge follows the controlled player's independent 100-unit reserve. START/Enter plus movement spends one unit per simulation update for the existing speed boost. Releasing turbo recovers two units per update after a 20-update delay. At zero, movement returns to normal; holding START cannot recover, and exhaustion clears at 25 units. The HUD changes T to X while exhausted. Walking without turbo can recover; pause and Dunk Cam freeze reserves.

Dunks spend 12 units, nearby shove attempts eight, and CPUs use the same budgets. Insufficient dunk reserve retains the charged jump shot; normal steals/blocks still work. Control handoff changes whose reserve is displayed, without transferring or refilling energy. Made baskets retain reserves and recover during inbounds. See [design and constants](../design/turbo.md).

## Verification

- `sh scripts/test.sh` passes with sanitizers and warnings as errors. New checks cover all six profiles, actual boost, complete depletion, held-empty lockout, delay/cap/rearm, pause/cinematic freeze, inbounds recovery, catch handoff, combo costs and CPU fairness. The 108,000-update contact soak also checks reserve bounds; all existing selection, shooting, finishing and inbound checks pass. See [host output](turbo/host-tests.txt).
- Official native ROM **a330be5fb230741e46e9c0af4b8e7ef0a78e8ccdd98f34805c0eb6937cdd0a99** builds as a valid 2 MiB GBC-only cartridge. The final build has the existing CLI deprecation/SDCC optimizer warnings and no errors or constant-overflow warnings.
- Genuine PyBoy 2.7.0 frames show MATUS's reserve at 100 → 51 → 0; continued START remains at 0; releasing recovers through 30 to 100. The empty HUD shows X and RELEASE TURBO. A pass changes control/HUD to MAGIC: MATUS retains 52 while MAGIC has 100.
- A right-basket MATUS dunk spends 46 → 34, keeps its gauge during Dunk Cam/pause, and returns with normal gauge tiles. A CPU BAMBI left-basket dunk also displays the named dunker's reserve. All inspected paused simulation bytes except previous input remain equal, including reserve/delay/exhaustion. VRAM bytes exactly match the intended opaque camera gauge and restored normal gauge assets.
- Official browser ROM **c2539df89cb4aa4fbc920d4118cce7326a0707e1395544a8ce69b9bb4eff15c2** was played in the existing built-in-browser tab. Full/partial/empty/recovering/full gauges were visually checked with MATUS. The prior game was saved before replacement; the preview is left natively paused, ready for P to resume.
- Official export plus `node scripts/check-native-sprite-budget.mjs` passes at 190/192 tiles per reserved bank span. Five bank-zero gauge tiles stay resident alongside the font; transition art uses at most 67 of its reduced 83-tile allowance. Normal cinematic pages remain at most 88 unique tiles/frame. No sprites are added.

## Evidence and limits

- [Depletion, exhaustion and recovery replay](turbo/depletion-recovery.gif)
- [Gauge through Dunk Cam, pause and return](turbo/dunk-meter.gif)
- [Pass/control-handoff replay](turbo/handoff.gif)
- [Browser full gauge](turbo/browser-full.png), [empty](turbo/browser-empty.png), [recovery](turbo/browser-recovering.png), [refilled](turbo/browser-recovered.png)
- [Build/runtime/branch/frame/VRAM receipts](turbo-evidence.json)

Clips are genuine retained native frames, with repeated images coalesced by the GIF exporter. Native and browser binaries have separate recorded identities. Physical Chromatic behavior remains untested; this practice build still lacks full match flow and hot-hand effects. Reserve capacity/cost/recovery choices are initial tuning values and preserve all supplied player ratings.

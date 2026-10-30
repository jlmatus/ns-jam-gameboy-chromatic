# Dunk Cam arena names

## Change

All six dunk sequences now place large muted teal player names in the upper arena, behind the athlete. Names remain in the HUD. Foreground tile masks preserve player, kit and basket colors. The left-basket composition pre-mirrors only the lettering so the final camera flip leaves it readable. Entry/exit wipes omit the word to protect resident font and turbo tiles.

## Verification

- Official native build succeeds: GBC-only 2 MiB ROM SHA-256 `82ebdc00b39c2dba889d9dcd8b03371112641092feb2cc6f295ba76f81aa1b1a`.
- Genuine PyBoy 2.7.0 play checks MATUS at the right basket (branch 1, frames 573–925), CPU BAMBI at the left basket (branch 2, frames 665–921), and long CHOMPA lettering (branch 3, frame 733). Names read normally at both ends, with natural arm occlusion.
- MATUS and BAMBI score and return to baseline inbounds with normal court/HUD tiles. The turbo gauge remains visible during the sequence. CHOMPA also returns to court.
- All 336 generated camera frames fit the tile limits: maximum 123/128 in the covered view and 67/83 during transitions. Official source export plus `node scripts/check-native-sprite-budget.mjs` passes at 190/192 sprite tiles.
- Official browser export succeeds with separate ROM SHA-256 `d0bf12d7c89295032899c6c2c6947cf3fd52b41b01cc086a0877b0c85756d12f`. The existing built-in-browser preview was refreshed after preserving the prior paused game state.
- `git diff --check` passes. This presentation change does not alter simulation rules; existing turbo/contact tests were not repeated.

## Captures and limits

[MATUS right-basket replay](dunk-names/matus-name.gif), [BAMBI left-basket replay](dunk-names/bambi-name.gif), [CHOMPA native frame](dunk-names/chompa.png), and [build/frame receipts](dunk-names-evidence.json).

Clips contain genuine retained native frames; repeated images are coalesced without interpolation. Source PNG studies are separate from emulator evidence. Physical Chromatic behavior remains untested; visual acceptance remains with the user.

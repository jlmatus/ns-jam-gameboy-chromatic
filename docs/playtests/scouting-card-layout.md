# Scouting Card Layout Verification

Implemented October 28, 2026 from the user's selected second layout concept.
The user also approved the six playstyle labels in `docs/design/playstyles.md`.
These labels are descriptions, not additional ratings or special abilities.

## Native UI

- Approved 32×32 source cells are placed at exactly 2× size: **64×64 portraits**.
- Names use double-size bold bitmap glyphs. Other text uses a heavier native
  font, high-contrast cream/navy colors, and fewer items per view.
- Main cards show speed and three useful skills for that player, each with a
  numeric value and ten-step amber meter. Original numeric ratings are unchanged.
- SELECT (keyboard P) cycles card → attack/movement ratings → defense/pressure
  ratings → card. All eight ratings remain available. B closes details first;
  A can confirm the player from any card/detail view. Existing lineup selection,
  duplicate prevention, difficulty, review, and pause/return behavior remain.
- Portrait backdrops contrast with hair. All approved opaque source cells and
  colors are retained. CHOMPA uses three native background palette families;
  one tile's transparent cells use the outline color to fit four colors per tile.
- Per-screen atlas usage is 58–108 tiles out of the 128 reserved UI tiles across
  two VRAM banks. Menu palettes use slots 3–6 as needed; UI palette 7 is preserved.
  The game restores its HUD font and window attributes when practice starts.

Editable inputs: `art/ui/scouting-cards.json`, approved portrait pixel JSON,
and bitmap glyphs in `art/prototype-pixels.json`. Regenerate the native source
with `node scripts/create-scouting-cards.mjs`. The generator rejects clipped text,
invalid source/featured ratings, palette overages, and tile-budget overages.
Generated concepts are not imported as flat screenshots or substituted for source art.

## Verification

`sh scripts/test.sh` passes with warnings as errors, ASan, and UBSan. It includes
all 1,080 lineup/difficulty combinations, all six detail-page cycles, held SELECT,
B close/back, A picking from details, page reset, and existing rules/rating tests.
No gameplay balance or rating values were changed in this presentation pass.

Final native ROM: 262,144 bytes, valid GBC-only cartridge/header/logo. SHA-256:
`f6301052dfa7f1df7f633ae76b8c5f1536c847359fa8e1645522df28e4eccbab`.
Official build succeeded. Stderr retains upstream Node DEP0190 and three existing
SDCC optimizer warning 110 messages; there were no new build failures.

PyBoy 2.7.0 recorded a clean boot through frame 928. Each of the **18** player/detail
views was inspected and compared to its deliberate native pixel layout/palette:
**zero mismatched pixels** in every view. The comparison checks actual ROM frames,
not source previews. It also confirmed lineup review, starting practice from a
selection made in details, restored court/body/HUD rendering, human movement,
pause, and returning to the large card.

Evidence: `scouting-card-evidence.json`, `scouting-card-flow-evidence.json`,
and `scouting-cards/`. The final recording is finalized with outcome passed.
`node scripts/check-scouting-capture.mjs` checks the retained captures after the
card generator has run. The comparison image places the chosen concept and
native screen at the same aspect and display size; see root `design-qa.md`.

Earlier iterations retained evidence of a missing horizontal font scale offset
and low hair/backdrop contrast. Both were fixed and the final ROM was rechecked.
Physical Chromatic testing remains unperformed. Native pixel matching does not
prove physical-display readability or represent final gameplay balance.

## Official browser verification

The separate official web export built successfully, ROM SHA-256
`e8cdf05948d79627687ec226de934d818c52f908f98e8ba72a8c5f047629595f`.
The built-in browser verified larger cards, both rating pages with P, X closing
ratings, arrow browsing, Z confirmation, teammate context, and backtracking.
The preview is left on MATUS's first picker. No errors/warnings appeared in the
checked browser console. Browser screenshots are retained in `scouting-cards/`.

The previous practice was saved at frame 23824, state
`ad62a93bfff97a60647f88bda2a490b5d9272f50ace5fd62062bc9c8cc12acf7`.
The task-owned preview service was restarted without deleting save history.
The prior plugin service reported an unclean close; this was not represented as
an acknowledged recording save. The previous recorder had been idle.

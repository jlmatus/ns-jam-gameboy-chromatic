# Selection Layout Review — October 28, 2026

The user requests larger portraits and more readable text before further gameplay changes.
No game code or approved portrait pixels are changed during this exploration.

1. **Player picker — functional, visually crowded.** Fresh native ROM capture at
   frame 120 (`02-current-native-picker.png`), SHA-256
   `05933341712489be07767a2aa6449ec512e4e6a642ed43d75c33e63934e6b7e9`.
   The 32×32 face occupies little space. One-pixel lettering and tightly stacked
   two-column ratings make scanning difficult. Long instructions create noise.
   Separate identity/selection from the full eight-rating view; strengthen font
   strokes, contrast, margins, and vertical spacing. Use 64×64 or 96×96 portrait
   placement made from the approved source cells.
2. **Existing browser game — paused human session, preserved.** The current tab
   shows a 9:24 practice game, not the picker (`01-current-paused-practice.png`).
   Its state was not resumed, reset, or navigated away to obtain menu evidence.
   A separate clean native emulator boot supplied the current picker capture.

Native capture uses cartridge SHA-256
`d40b377142f3b6688709f3c74975a07ad9d5c9a0fdeeaa823d21acf4332bb017`.
The temporary emulator recording was finalized and closed. The handheld's
160×144 framebuffer is a fixed constraint; browser enlargement does not improve
handheld typography. Screenshots establish visible density and contrast concerns,
not full accessibility or physical-device legibility compliance.

## Concepts

Three independent generated image concepts compare a portrait-first card, a scouting
card with a few primary ratings, and a roster carousel. Each keeps the approved
MATUS identity, native pixel aesthetic, A confirmation/B back navigation, and
separate access to all ratings. Generated concepts are design references rather
than native game assets or verified executable screens. The user selected the second layout and its native implementation is now verified; gameplay bugs remain the subsequent task.

## Displayed concept order

1. `concept-1-portrait-spotlight.png`
2. `concept-2-scouting-card.png`
3. `concept-3-roster-carousel.png`

These images vary layout and font emphasis. Generated portraits, texture, and
colors are not replacements for approved native source art. The chosen layout
will be adapted to the fixed resolution, tile/palette budget, exact approved
portrait cells, and a genuinely readable native font. The user selected option 2. The native scouting-card implementation and approved playstyle labels are documented in `docs/playtests/scouting-card-layout.md` and `docs/design/playstyles.md`.

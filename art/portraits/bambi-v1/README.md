# BAMBI — portrait concept v1

Standalone face portrait based on the user-provided screenshot, matching the approved MATUS portrait's 32×32 size and three-color palette. **Portrait approved by the user.** No uniform has been invented, and no game project has been created or modified.

## Files

- `bambi-32.png`: transparent 32×32 portrait, with three opaque colors.
- `bambi-preview.png`: exact 12× nearest-neighbor enlargement.
- `pixels.json`: editable palette and rows, encoded as `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `bambi-native-source.png`: matching cells mapped to GB Studio's three sprite source shades and green transparency marker; not a registered sprite.
- `design-reference.png`: original built-in Image Generation study.
- `blank-grid-32.png` and `generated-grid.png`: exact source chart and generated reconstruction.
- `prompts.md`: exact generation prompts and reference roles.
- `grid-review/`: unmodified diagnostic report, candidate outputs, review crops, and separate manual adjudication.

## Review and integration

The chart contains 32×32 cells with all 33 guides acquired on each axis. All 511 occupied cells passed the diagnostic. The raw report flagged 64 empty cells for guide-edge fidelity; these were reviewed and accepted separately in `grid-review/manual-review.json`. The raw result remains `review-grid-fidelity`, not a zero-flag pass.

The final PNG maps each cell to an exact palette index and binary alpha, without shrinking the whole chart. Its editable data reproduces the reviewed portrait. Future game integration requires a compatible 32×32 layout or background composition, palette assignment, and runtime validation. No asset import, ROM build, emulator test, or hardware check has been performed.

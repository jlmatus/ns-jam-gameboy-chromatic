# TORRES — portrait concept v1

Standalone face portrait based on the user-provided photograph, matching the approved portraits' 32×32 size and three-color palette. **Portrait approved by the user.** Short textured hair, strong eyebrows, a connected mustache, and a full trimmed beard distinguish TORRES. Clothing from the photograph is omitted; no uniform has been invented.

## Files

- `torres-32.png`: transparent 32×32 portrait with three opaque colors.
- `torres-preview.png`: exact 12× nearest-neighbor enlargement.
- `pixels.json`: editable palette and rows, encoded as `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `torres-native-source.png`: matching cells mapped to GB Studio's three sprite source shades and green transparency marker; not a registered sprite.
- `design-reference.png`: original built-in Image Generation study.
- `blank-grid-32.png` and `generated-grid.png`: source chart and generated reconstruction.
- `prompts.md`: exact generation prompts and reference roles.
- `grid-review/`: original diagnostics, candidate images, and any separately recorded visual adjudication.

## Pixel review

The diagnostic acquired all 1,024 cells and 33 guides on each axis, but reported `reject-grid-fidelity`: 12 rejected cells and 139 review cells. Eight rejected cells contained artwork, including subdivided eye glints and split ear shading. The raw report and images remain unchanged.

The final version uses explicit native pixel cleanup at 11 eye, ear, temple, and beard cells. Five palette indices differ from the diagnostic candidate; the other decisions flatten mixed source cells to the chosen whole-cell color. Remaining flags were reviewed as guide-edge artifacts. Decisions and source crops are recorded in `grid-review/manual-review.json`; this is not a passed exact extraction. The final PNG has three opaque colors, binary alpha, and 533 transparent cells.

## Integration

The 32×32 portrait requires a compatible custom layout or background composition, rather than the standard 16×16 avatar/static profile. Future integration must assign the displayed palette and verify hardware limits. The game remains in planning. No asset import, ROM build, emulator test, or hardware check has been performed.

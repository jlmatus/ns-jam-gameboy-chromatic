# MAGIC — portrait concept v1

Standalone face portrait based on the user-provided photograph, using the approved MATUS and BAMBI portraits' 32×32 size and three-color palette. **Portrait approved by the user.** Rectangular glasses, side-swept hair, and a broad smile distinguish MAGIC. No uniform has been invented.

## Files

- `magic-32.png`: transparent 32×32 portrait with three opaque colors.
- `magic-preview.png`: exact 12× nearest-neighbor enlargement.
- `pixels.json`: editable palette and rows, encoded as `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `magic-native-source.png`: identical cells mapped to GB Studio's three sprite source shades and green transparency marker; not a registered sprite.
- `design-reference.png`: original built-in Image Generation study.
- `blank-grid-32.png` and `generated-grid.png`: source chart and generated reconstruction.
- `prompts.md`: exact generation prompts and reference roles.
- `grid-review/`: original diagnostics, candidate outputs, and any separately recorded visual adjudication.

## Pixel review

All 1,024 chart cells and 33 guides per axis were acquired. The raw diagnostic reported `reject-grid-fidelity`: seven empty cells and one occupied chin cell were rejected, and 92 empty cells needed review. Original flags remain in `grid-review/magic.json`.

The empty-cell flags were visually reviewed as guide-edge artifacts. Chin cell `(13,28)` was deliberately redrawn as solid light to continue the jaw highlight. The final image therefore includes native pixel cleanup, rather than claiming a passed exact extraction. See `grid-review/manual-review.json` for the decision and source crops. The PNG uses exactly three opaque colors, binary alpha, and 507 transparent cells.

## Integration

The approved 32×32 source cells are integrated as native window tiles, with their displayed palette verified in ROM menu frames. Physical hardware remains untested.

Integrated into the native selection menu using the approved source cells. See `docs/design/ratings.md` for native palette/tile packing.

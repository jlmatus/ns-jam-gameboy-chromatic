# MEAT — default portrait v1

**Approved default portrait; jersey 00.** No photo reference is available, so the selected appearance is an accepted invented design rather than a verified likeness. The lively expression is inspired by the confirmed signature label **BIG ENERGY**; no gameplay behavior is implied. Preserve jersey `"00"` as a string.

The portrait follows the approved roster's 32×32 size and three-color style. Jersey 00 is supplied by the user. No uniform or player ratings have been invented or changed.

## Files

- `meat-32.png`: transparent 32×32 portrait with three opaque colors.
- `meat-preview.png`: exact 12× nearest-neighbor enlargement.
- `pixels.json`: editable palette and rows, encoded as `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `meat-native-source.png`: matching cells mapped to GB Studio's three sprite source shades and green transparency marker; not a registered sprite.
- `design-reference.png`: original built-in Image Generation study, using BAMBI only as a style reference.
- `blank-grid-32.png` and `generated-grid.png`: source chart and generated reconstruction.
- `prompts.md`: exact generation prompts and reference roles.
- `grid-review/`: raw diagnostics, candidate images, and any separately recorded native corrections.

## Pixel review

All 1,024 cells and 33 guides per axis were acquired. The raw diagnostic reported `reject-grid-fidelity`: six rejected cells and 108 review cells. The occupied chin rejection and cheek review cell received explicit native whole-cell cleanup; one palette index differs from the candidate. The 112 flagged empty cells were reviewed as margin/guide-edge artifacts. Original reports, crops, and decisions remain in `grid-review/`; this is not a passed exact grid extraction.

The final portrait has exactly three opaque colors, binary alpha, and a matching editable source.

## Integration

The approved invented design remains the default. Its exact 32×32 source cells are integrated as native window tiles and verified in the built ROM. Physical hardware remains untested.

Integrated into the native selection menu using the approved source cells. See `docs/design/ratings.md` for native palette/tile packing.

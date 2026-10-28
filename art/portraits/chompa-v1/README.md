# CHOMPA — portrait concept v1

Standalone face portrait based on the user-provided photograph, matching the approved portraits' 32×32 size and three-color palette. **Earlier study; not approved.** See [CHOMPA v2](../chompa-v2/README.md) for the current revision. Thick rectangular glasses, very short hair, fuller cheeks, a toothy smile, and light jaw stubble distinguish this study. The hand, clothing, and room are omitted; no uniform has been invented.

## Files

- `chompa-32.png`: transparent 32×32 portrait with three opaque colors.
- `chompa-preview.png`: exact 12× nearest-neighbor enlargement.
- `pixels.json`: editable palette and rows, encoded as `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `chompa-native-source.png`: matching cells mapped to GB Studio's three sprite source shades and green transparency marker; not a registered sprite.
- `design-reference.png`: original built-in Image Generation study.
- `blank-grid-32.png` and `generated-grid.png`: source chart and generated reconstruction.
- `prompts.md`: exact generation prompts and reference roles.
- `grid-review/`: original diagnostics, candidate images, and any separately recorded visual adjudication.

## Pixel review

All 1,024 chart cells and 33 guides per axis were acquired, but the raw diagnostic reported `reject-grid-fidelity`: 44 rejected cells and 135 review cells. Sixty flagged cells contained artwork; 119 were empty margins or guide edges. Original reports and candidates remain unchanged.

Flagged occupied interiors were inspected in `grid-review/occupied-review-cells.png` and explicitly redrawn as whole native cells. The glasses, smile, ear shading, and neck framing received deliberate cleanup; 23 final cells differ from the diagnostic candidate. The extended neck was redrawn to restore a transparent bottom border. This is native pixel authoring, not a passed exact grid extraction. Decisions are recorded in `grid-review/manual-review.json`.

The final portrait uses exactly three opaque colors, binary alpha, and 489 transparent cells. It contains no off-palette blue lens reflections or fine stubble marks.

## Integration

The 32×32 portrait requires a compatible custom layout or background composition, rather than the standard 16×16 avatar/static profile. Future integration must assign the displayed palette and verify hardware limits. The game remains in planning. No asset import, ROM build, emulator test, or hardware check has been performed.

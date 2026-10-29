# CHOMPA — default portrait v2

Revised 32×32 face portrait based on the closer beanie photograph. **Approved default portrait; jersey 74.** Fuller cheeks and jaw, hooded eyes, angular glasses, a relaxed smile, chin stubble, and a lime beanie distinguish the revised likeness. `chompa-32.png` is the selected default color portrait; the three-color version is an integration variant. V1 artwork remains intact for comparison.

## Files

- `chompa-32.png`: transparent color concept with four opaque colors, adding lime to the established ink and skin shades.
- `chompa-preview.png`: exact 12× enlargement of the color concept.
- `chompa-32-three-color.png` and `chompa-preview-three-color.png`: restricted variant retaining lime, ink, and one skin tone.
- `pixels.json`: editable whole-cell rows plus color, three-color, and native-source palette mappings.
- `chompa-native-source.png`: three-color variant encoded in native sprite source shades and the green transparency marker; not a registered sprite.
- `design-reference.png`, `blank-grid-32.png`, and `generated-grid.png`: preserved generation studies and source chart.
- `prompts.md`: exact built-in Image Generation prompts and reference roles.
- `grid-review/`: raw diagnostics and any separately recorded pixel corrections.

## Pixel review

The diagnostic acquired all 1,024 cells and 33 guides per axis, but reported `reject-grid-fidelity`: 19 rejected occupied cells, one occupied review cell, and 26 empty review cells. Original reports and source images remain unchanged.

The final version uses deliberate native pixel authoring for the glasses, cap fill and brim seam, chin beard, and short neck. Fifty-eight cell indices differ from the diagnostic candidate. Mixed source features were reviewed and replaced with intentional whole-cell shapes; this is not a passed exact grid extraction. See `grid-review/manual-review.json` for source flags and authoring decisions.

The restricted palette retains the green beanie by merging skin shadow into skin light. It therefore has flatter facial shading than the four-color concept.

## Integration

The full-color default is integrated as native background/window tiles, whose four-color palette preserves both skin shades and the green beanie. The three-color sprite variant remains available. Native ROM menu frames have been verified; physical hardware remains untested.

Integrated into the native selection menu using the approved source cells. See `docs/design/ratings.md` for native palette/tile packing.

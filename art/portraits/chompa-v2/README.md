# CHOMPA — portrait revision v2

Revised 32×32 face portrait based on the closer beanie photograph. **Pending user approval.** Fuller cheeks and jaw, hooded eyes, angular glasses, a relaxed smile, chin stubble, and a lime beanie distinguish the revised likeness. V1 artwork remains intact for comparison.

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

The color concept's four opaque colors exceed one hardware object's three opaque colors. The restricted variant is provided for future sprite-palette evaluation. A 32×32 portrait also needs a compatible custom layout or background composition; it is not a standard 16×16 avatar/static import. No project import, ROM build, or runtime verification has been performed. The game remains in planning.

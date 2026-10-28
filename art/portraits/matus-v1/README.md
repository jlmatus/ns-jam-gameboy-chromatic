# MATUS — portrait concept v1

Standalone character-face exploration based on the user-provided `joe.jpeg`. No uniform has been invented. **Portrait approved by the user.** The game remains in planning; this portrait has not been integrated.

## Review files

- `matus-32.png`: 32×32 transparent PNG with exactly three opaque colors.
- `matus-preview.png`: 12× nearest-neighbor enlargement of that exact PNG.
- `pixels.json`: editable palette and explicitly authored rows. Each row is `[starting_x, pixel_symbols]`; unspecified cells are transparent.
- `matus-native-source.png`: identical pixel layout mapped to GB Studio's three sprite source shades and green transparency marker. This is a source candidate, not a registered sprite.
- `design-reference.png`: original generated facial study.
- `generated-grid.png`: original generated reconstruction attempt, preserved without alteration.
- `prompts.md`: exact prompts used with built-in Image Generation.
- `grid-review/`: raw diagnostic results and diagnostic candidate images; these are not final artwork.

## Method and limits

The ModRetro Chromatic pixel-art skill guided the workflow. Image Generation produced an identity study and attempted a 32×32 chart reconstruction. The chart actually contained 39×39 cells; the diagnostic reported `unscored-geometry`. It is not accepted as an exact grid recovery. Original flags and candidate outputs remain available for review.

The final 32×32 portrait uses the skill's native-authoring fallback: explicit whole-pixel cells, designed from the reference studies rather than resized from their raster output. No chart cells were silently removed or treated as a passed recovery.

The approved artwork is preserved as the style reference for subsequent player portraits. The 32×32 canvas does not match GB Studio's standard 16×16 static sprite or avatar profile. Future integration must select a compatible custom layout or background composition, assign the displayed palette, and check hardware limits. No asset import, ROM build, emulator test, or physical-device check has been performed.

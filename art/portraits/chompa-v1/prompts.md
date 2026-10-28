# CHOMPA portrait generation prompts

Method: built-in Image Generation, following the ModRetro Chromatic pixel-art skill. The photograph supplies identity; approved MAGIC supplies style and palette.

## Grid reconstruction

Inputs: `blank-grid-32.png` (edit target; 1254×1254, origin 19, pitch 38) and `design-reference.png` (content reference only).

```text
Use case: stylized-concept.
Image 1 is the EDIT TARGET: exact blank 32x32 chart, 1254x1254 source pixels. Image 2 is CHOMPA's facial design reference ONLY.
Rebuild and SIMPLIFY CHOMPA's face by recoloring WHOLE EXISTING SQUARES of Image 1. Never paste an image underneath the grid. Preserve all 33 vertical and 33 horizontal black guides, spacing, straightness and continuity, and the white outer margin. Grid origin (19,19), pitch 38 source pixels. Regardless of output size, there must remain EXACTLY 32 squares per axis, never add more guides.
Each entire square interior must contain ONE flat color up to all four black boundaries. Only artwork swatches: dark ink #22303A, shadow #B96D47, skin light #F2C18D. Empty cells opaque magenta #FF00FF. Pure black ONLY for guides. Keep chart opaque.
The centered head is at most 26 cells wide by 28 high, with 2 empty rows above/below and at least 3 empty columns on left/right. Head and a hint of neck only. No hands, clothes or shoulders.
Preserve CHOMPA's broad rounded face, full lower cheeks, tall forehead, very short neat dark crop with gently receding rounded hairline, thick rectangular glasses with softened corners, visible friendly eyes, slight head tilt, broad toothy smile. Keep the tilt subtle enough for a small selection portrait. Suggest light stubble as a simple shadow cluster along the jaw; do NOT draw a thick full beard, goatee or fine stubble dots. Flatten hair to dark ink without individual strands or highlight speckles.
Use coarse budgets: short hair 3-4 cells tall, forehead 5 cells, glasses lens interiors 4x2 cells with 1-cell dark frames and bridge, pupils one dark cell, smile a 5-6-cell skin-light strip with simple dark corners. Teeth use the declared skin-light color, NOT white.
Every feature including pupils, outlines and stubble must use whole squares. Omit detail instead of subdividing cells. No tiny white eye glints, reflections, curves, anti-aliasing, gradients, extra shades, dithering, textures, lettering or new guides. The final image must visibly be a colored-in graph-paper mosaic, all features aligned to the complete original grid.
```

## Coarse design

```text
Use case: stylized-concept.
Create one coarse pixel-art FACE portrait of CHOMPA for NICASOURCE JAM. Image 1 is CHOMPA's photographic identity reference ONLY. Image 2 is approved MAGIC, a 32x32 STYLE and PALETTE reference ONLY. CHOMPA also wears glasses but must retain his own likeness; do not copy MAGIC's face or long swept hairstyle.
Identity: broad oval/rounded face with fuller lower cheeks and chin, very short close-cropped dark hair, mildly receding rounded hairline and a tall open forehead, thick dark navy rectangular eyeglasses with softly rounded corners and substantial dark temples, friendly eyes visible through the lenses, softly rounded nose, cheerful broad toothy smile, faint short stubble along the jaw and below the chin. Suggest stubble with flat skin-shadow clusters, not a full solid dark beard or goatee. Head is slightly tilted as in the photo, looking toward the viewer. Head and a hint of short neck only; no hand, peace sign, shirt, shoulders, room, computer, jewelry or other objects.
Match the approved low-resolution portrait style: native 32 by 32 logical pixels enlarged with hard square blocks, head footprint about 24 cells wide by 27 high centered with empty margins. Hair 3 cells tall, forehead 5 cells, each glasses lens about 6x4 cells with 1-cell dark frames and a bridge, pupils 1 cell, nose 3 cells, toothy smile 6 cells. Use one light strip for the teeth without tiny tooth divisions. Glasses, smile, close haircut and fuller face are the priorities.
Use exactly THREE flat opaque artwork colors: dark ink #22303A, warm skin shadow #B96D47, skin light #F2C18D. Teeth use the skin-light swatch; no extra white. Background solid opaque magenta #FF00FF. Large flat connected clusters only. No visible grid, labels, text, uniforms, extra colors, lens reflections, gradients, dithering, antialiasing, realistic lighting, texture, fine stubble dots, tiny details, or high-resolution portrait rendering.
```

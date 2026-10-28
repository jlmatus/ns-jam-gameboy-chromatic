# MATUS portrait generation prompts

Method: built-in Image Generation, following the ModRetro Chromatic pixel-art skill's two-stage recoverable-grid workflow.

## Coarse design

Reference: user-provided `joe.jpeg`, used for facial identity only.

```text
Use case: stylized-concept.
Create ONE pixel-art character FACE portrait of MATUS for NICASOURCE JAM, a Game Boy Color arcade basketball game. Image 1 is the photographic identity reference only. Closely preserve his recognizable facial proportions: short upward spiked dark hair with swept top, mature broad forehead and slightly receding temples, strong dark eyebrows, dark brown eyes, warm medium skin, broad nose, subtle self-assured closed-mouth smile, thin mustache connected to a short goatee around the mouth and chin, wider cheeks and softly squared jaw. Face nearly forward with the slight tilt from the photo. Head and short neck ONLY; omit hand, watch, shirt, shoulders. No invented basketball uniform.
Render as a deliberately COARSE native 32 by 32 pixel design enlarged to 1024 square with hard nearest-neighbor square blocks; the head occupies approximately 24 by 28 logical cells, centered with empty margins. Eyes and mouth features must be made of single whole logical pixels and chunky connected clusters; no feature smaller than one logical pixel. Hair silhouette and goatee are the identity priorities. Only THREE flat artwork colors: ink #22303A, warm skin shadow #B96D47, skin light #F2C18D. Empty background is solid opaque magenta #FF00FF. No visible grid, no text, no border, no gradients, antialiasing, extra shades, texture, rendered lighting, fine dithering, realistic painting, or tiny detail. This is a small readable 1990s handheld game selection portrait, not a polished high-resolution portrait.
```

## Grid reconstruction

Inputs: the plugin's blank 32×32 grid, then the generated design as a content reference.

```text
Use case: stylized-concept.
Image 1 is the EXACT BLANK 32 BY 32 CHART and is the EDIT TARGET. Image 2 is MATUS's portrait design reference ONLY, NOT an image to paste under a grid. Reconstruct and SIMPLIFY this face by recoloring WHOLE EXISTING CELLS of Image 1.
Preserve Image 1's square 1024x1024 layout: white margins, grid origin (32,32), pitch 30, exactly 33 vertical and 33 horizontal pure-black guides enclosing exactly 32x32 cells. Every cell interior is completely filled with ONE flat color all the way to its four black boundaries. Keep all guide lines straight, continuous, equally spaced, and unbroken.
Use EXACTLY these swatches: dark ink #22303A, warm shadow #B96D47, skin light #F2C18D; empty cells stay opaque magenta #FF00FF. Pure black is reserved ONLY for guides. No other artwork colors. No real transparency in this chart.
The face in Image 2 has too many fine steps: SIMPLIFY to a genuine 32x32 face. Head footprint around 24 cells wide by 28 cells tall, centered with an empty border. Keep recognizability through his short spiked upswept dark hair, wide forehead, strong eyebrows, broad nose, slightly tilted friendly confident closed-mouth smile, thin mustache and goatee. Hair spikes 1-2 cells each, each eye about 3 cells wide, eyebrows about 4 cells, nose about 3 cells, mouth about 5 cells; goatee broad connected dark clusters. Head and a hint of neck only. No shirt, hand, shoulders, lettering, framing or uniform.
All features including eyes and outlines MUST use whole grid squares only. Drop a detail rather than subdividing any cell. No image beneath the chart, no split squares, curves, tiny marks, antialiasing, gradients, dithering, or extra guide lines. The final output must visibly look like a colored-in graph paper mosaic of this face, with the complete original grid visible.
```

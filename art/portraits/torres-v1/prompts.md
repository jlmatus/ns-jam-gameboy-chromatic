# TORRES portrait generation prompts

Method: built-in Image Generation, following the ModRetro Chromatic pixel-art skill. The photograph supplies identity; the approved BAMBI portrait supplies style and palette.

## Grid reconstruction

Inputs: `blank-grid-32.png` (edit target; 1254×1254, origin 19, pitch 38) and `design-reference.png` (content reference only).

```text
Use case: stylized-concept.
Image 1 is the EDIT TARGET: exact blank 32x32 chart at 1254x1254 source pixels. Image 2 is TORRES's facial design reference ONLY.
Rebuild and SIMPLIFY the face by recoloring whole existing squares of Image 1. Never paste an image under a grid. Preserve the original 33 vertical and 33 horizontal black guides, equally spaced, straight and continuous, and white outer margin. Grid origin (19,19), source pitch 38. Regardless of output resolution, retain exactly 32 squares per axis; do not add guides.
Each whole cell interior gets one flat color all the way to its black boundaries. Artwork colors ONLY: dark ink #22303A, shadow #B96D47, skin light #F2C18D. Empty cells remain opaque magenta #FF00FF. Pure black is reserved ONLY for guides. The chart is opaque. Remove the reference study's gray hair highlights; all hair is the dark ink swatch.
TORRES head only, centered with 2 empty rows above/below and at least 3 empty columns left/right, footprint at most 26 wide by 28 tall. Preserve short cropped slightly textured hair with a low fringe, close faded sides suggested by flat shadow clusters, thick brows, almond-shaped eyes, calm confident closed-mouth expression, slightly turned face as in the reference, full angular dark beard with connected mustache and small central soul patch. The beard follows the full jaw, not a goatee-only silhouette. No glasses, no grin, no shirt, no suit or shoulders.
Use coarse feature budgets: hair 4-5 cells tall, brows 4 cells each, eyes 2-3 cells, nose 3 cells, mouth 5 cells, beard a large connected dark cluster with skin opening at the lips and a one-cell soul patch.
Every facial feature and contour must use WHOLE squares. Drop detail that does not fit. No subdivided cells, curves, tiny marks, antialiasing, gradients, dithering, textures, extra shades, labels or new guides. Output should visibly be a colored-in graph-paper mosaic on the complete original grid.
```

## Coarse design

```text
Use case: stylized-concept.
Create one coarse pixel-art FACE portrait of TORRES for NICASOURCE JAM. Image 1 is TORRES's photographic likeness reference ONLY. Image 2 is the approved BAMBI portrait, a low-resolution STYLE and PALETTE reference ONLY; do not copy BAMBI's face or beard pattern.
TORRES identity: short dark slightly wavy/curly hair with a low textured fringe and close faded sides, thick dark eyebrows, almond-shaped dark eyes, broad straight nose, calm confident closed-mouth expression, strong squared lower face with a FULL neatly trimmed dark beard and connected mustache, defined diagonal beard cheek lines and a small soul patch. Head is slightly turned as in the photo, eyes toward viewer. Show head with a little neck only, no suit, shirt, tie or shoulders. No glasses.
Match the approved 32x32 logical-pixel style, enlarged with sharp nearest-neighbor blocks. Whole head including beard fits about 24 cells wide by 27 high, centered with empty margins. Large flat clusters, short hair about 4 cells tall, brows 4 cells each, eyes 2-3 cells, nose 3 cells, mouth 5-6 cells, beard a bold connected silhouette following the jaw with skin opening around the closed mouth. Avoid a large open smile or goatee-only beard.
Use exactly THREE flat opaque artwork colors: dark ink #22303A, warm skin shadow #B96D47, skin light #F2C18D. Empty background solid opaque magenta #FF00FF. No visible grid, labels, text, extra shades, gradients, dithering, smoothing, antialiasing, tiny details, realistic rendering, fine hair strands, texture or scenery.
```

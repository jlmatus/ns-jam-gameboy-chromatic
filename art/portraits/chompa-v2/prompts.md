# CHOMPA v2 generation prompts

Built-in Image Generation under the ModRetro Chromatic pixel-art workflow. The new close-up photo supplies facial identity and the lime beanie; v1 supplies only the existing coarse portrait medium. V1 and MAGIC facial proportions must not be reused.

## Grid reconstruction

Inputs: `blank-grid-32.png` (edit target; 1254×1254, origin 19, pitch 38) and `design-reference.png` (content reference only).

```text
Use case: identity-preserve.
Image 1 is the EDIT TARGET: exact blank 32x32 chart, 1254x1254 pixels, grid origin (19,19), pitch 38. Image 2 is the revised CHOMPA design, CONTENT REFERENCE ONLY.
Rebuild and simplify this specific man's face as a coarse whole-cell portrait by recoloring existing squares of Image 1. Never paste an illustration under a grid. Preserve all 33 horizontal and 33 vertical pure-black guides continuous, straight and evenly spaced, and the white outer margin. Exactly 32 squares per axis regardless of output resolution; no new guides.
Each complete square interior must have just ONE flat color filling it to its four black boundaries. Artwork swatches ONLY: ink #22303A, skin shadow #B96D47, skin light #F2C18D, lime beanie #A6CE39. Empty cells remain opaque magenta #FF00FF. Pure black is ONLY for guides. Chart opaque.
CHOMPA's identity comes from his BROAD LOWER FACE, full rounded cheeks and chin, hooded eyes with heavier upper lids, angular rectangular glasses, relaxed asymmetric toothy smile, visible short beard dense under the chin. Keep a subtle head tilt but no distorted oversized grin. Include the LIME BEANIE with a thick folded brim and smooth snug dome. Simplify the design's overfine beard marks and glasses detail: chin beard is 1-2 connected dark/shadow rows, cheeks mostly clear; never scatter dots or checkerboards. Beanie is one lime fill plus a simple dark outline/brim seam, NO knit texture.
Entire head with beanie and tiny neck fits at most 26 cells wide and 28 cells high, centered with TWO EMPTY ROWS at top and bottom, at least 3 empty columns left/right. No clothes, shoulders, text or background objects.
Use large flat clusters: hat about 6 rows including brim, whole-cell lens interiors about 4x2 with one-cell dark rims and bridge, single-cell pupils, nose 3 cells, teeth a short 4-5-cell light strip. Teeth use skin light, not white. Facial fullness and heavier chin beard must distinguish this man from the earlier MAGIC portrait. Never copy a generic narrow jaw.
Every feature must be expressed in whole existing squares, including eyes, beard and contours. Omit detail that cannot fit. No subdivided cells, curves inside cells, gradients, anti-aliasing, extra swatches, fine marks, reflections, dithering or texture. Output must visibly be the complete original chart colored in as a graph-paper mosaic.
```

## Coarse revised design

```text
Use case: identity-preserve.
Create a substantially improved coarse pixel-art face portrait of CHOMPA for NICASOURCE JAM using the NEW close-up photo (Image 1) for likeness. Image 2 is the previous unsuccessful CHOMPA v1, only a medium/resolution reference: it looked too similar to another player and must NOT dictate the new facial anatomy.
Make CHOMPA recognizable from Image 1: a notably fuller, broad and slightly elongated rounded lower face; soft heavy cheeks and a substantial squared-rounded chin; hooded eyes with heavier upper lids; angular dark rectangular eyeglasses with slightly softened lower corners, thick rims and temples; a broad rounded nose; relaxed asymmetric toothy smile with a small strip of teeth; short but clearly visible dark chin/jaw stubble, denser UNDER the chin, with a sparse upper lip rather than a thick mustache. Head tilted slightly as in the photo, nearly frontal, eyes toward viewer.
Include his distinctive bright LIME GREEN KNIT BEANIE with a thick folded brim, snug rounded silhouette and slight asymmetric slouch. No visible tall swept hair. Express the cap in a single lime color with a few whole-cell dark contour accents, NO realistic knit texture. Head only, a tiny neck; omit clothing, shoulders, background and text.
Same hard chunky 32x32 logical-pixel game-portrait medium as Image 2, enlarged with sharp nearest-neighbor square blocks. Entire head plus beanie fits about 26x28 cells, centered with clear empty margins. Head and jaw proportions and chin beard are as important as the hat. Oversized generic smiling cartoon features are wrong. Glasses lenses about 5x3 cells each inside 1-cell rims, pupils whole cells, no fine reflections.
FOUR flat opaque artwork swatches ONLY: ink #22303A, skin shadow #B96D47, skin light #F2C18D, beanie lime #A6CE39. Solid opaque magenta #FF00FF empty background. The extra lime swatch is for this color concept; a separate restricted-palette version will be made later. No white teeth swatch; teeth use skin light. No visible grid, no gradients, shading blends, antialiasing, fine texture, dithering, tiny marks, lens reflections, logos or captions.
```

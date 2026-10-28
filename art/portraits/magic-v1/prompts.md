# MAGIC portrait generation prompts

Method: built-in Image Generation, following the ModRetro Chromatic pixel-art skill. The photograph supplies identity; the approved BAMBI portrait supplies style and palette.

## Grid reconstruction

Inputs: `blank-grid-32.png` (edit target; 1254×1254, origin 19, pitch 38) and `design-reference.png` (content reference only).

```text
Use case: stylized-concept.
Image 1 is the EDIT TARGET: exact blank 32x32 chart at 1254x1254 source pixels. Image 2 is MAGIC's face design reference ONLY.
Rebuild and SIMPLIFY MAGIC's face by recoloring WHOLE EXISTING SQUARES of Image 1. Never paste an illustration beneath the grid. Preserve all 33 vertical and 33 horizontal black guides, their spacing, straightness and continuity, and the white outer margin. Grid origin (19,19), source pitch 38. Regardless of output resolution, retain EXACTLY 32 squares per axis, never add guides.
One flat color per whole square, filling the entire interior to all four black boundaries. Only artwork colors: dark ink #22303A, warm shadow #B96D47, skin light #F2C18D. Empty squares remain solid opaque magenta #FF00FF. Pure black is ONLY for guides. Keep the chart opaque.
Center a head-only portrait about 24 cells wide by 27 cells high with two empty rows above and below and at least 3 empty columns left/right. No shoulders or clothes.
Preserve MAGIC's distinct oval face, short side-swept hair, dark rectangular eyeglasses with visible eyes, a 1-cell bridge joining two rectangular frames, friendly broad toothy smile, and gently squared clean-shaven jaw. Simplify eyeglasses to two roughly 6x4-cell rectangles with 1-cell dark frames and skin-light lens interiors, each pupil one dark cell. Eyebrows chunky whole cells. Use the same skin-light swatch for a single 5-7-cell teeth strip; do not add white. Hair should be a simple short 4-5-cell-tall swept shape, no spikes, no large beard, no mustache.
All contours, pupils and smile details must consist of whole squares. Drop secondary detail if necessary. No cell subdivisions, smooth curves, extra shades, gradients, glass reflections, textures, antialiasing, tiny marks, dithering, labels or new guides. The complete original chart must remain visible as a colored-in graph-paper mosaic.
```

## Coarse design

```text
Use case: stylized-concept.
Create one coarse pixel-art character FACE portrait of MAGIC for NICASOURCE JAM. Image 1 is the photographic identity reference ONLY. Image 2 is BAMBI's approved portrait, a STYLE and PALETTE reference ONLY. Keep MAGIC's distinct likeness; do not copy BAMBI's round face or beard.
Identity: short dark hair swept neatly to one side, close sides, oval face with a gently squared jaw, dark thick brows, large dark rectangular prescription eyeglasses with a narrow bridge, friendly eyes visible through the lenses, broad happy smile showing a single light strip of teeth, prominent cheeks, warm tan skin. Nearly frontal head with small ears and a hint of neck, no shirt or shoulders. Facial hair is only faint stubble in the reference; omit it at this coarse resolution rather than adding a goatee or full beard.
Style: native 32x32 logical pixels enlarged with hard square blocks, matching Image 2. Head fits roughly 22-24 cells wide and 27 cells high, centered with empty margins. Use chunky flat clusters, no tiny detail. Hair silhouette about 5 cells tall, each glasses frame roughly 6 cells wide by 4 cells high with 1-cell dark borders and 1-cell bridge, clear skin-colored lens interiors, single-cell eyes, nose 3 cells wide, smile 7 cells wide. Glasses and smile are the priority identity cues.
Exactly three flat opaque artwork colors: dark ink #22303A, warm shadow #B96D47, skin light #F2C18D. Use skin light for the teeth, NOT an extra white swatch. Empty background solid opaque magenta #FF00FF. No visible grid, text, uniform, clothing, scenery, glass shine, lens reflections, gradients, dithering, antialiasing, fine texture, tiny marks, realistic lighting, or high-resolution illustration.
```

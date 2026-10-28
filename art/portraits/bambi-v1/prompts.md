# BAMBI portrait generation prompts

Method: built-in Image Generation, following the ModRetro Chromatic pixel-art workflow. The approved MATUS portrait is a style reference; BAMBI's photo supplies facial identity.

## Grid reconstruction

Inputs: `blank-grid-32.png` (edit target; 1254×1254, origin 19, pitch 38, 32 cells) and `design-reference.png` (content reference only).

```text
Use case: stylized-concept.
Image 1 is the EDIT TARGET: an exact blank 32x32 pixel chart, 1254x1254 source pixels. Image 2 is BAMBI's facial design reference ONLY.
Rebuild and simplify BAMBI as a 32x32 pixel portrait by recoloring WHOLE EXISTING SQUARES in Image 1. Do not paste a picture under the grid. Keep Image 1's 33 horizontal and 33 vertical black guides, unchanged, continuous and evenly spaced. Grid centers are origin (19,19), pitch 38 source pixels, enclosing 32x32 cells. Regardless of output image resolution there must be exactly 32 squares per axis, never add more.
Each square gets exactly ONE flat color filling its entire interior up to all four black guides. Only these artwork colors: ink #22303A, shadow #B96D47, light #F2C18D. Empty squares stay solid opaque magenta #FF00FF. Pure black is reserved exclusively for grid guides; preserve white outer margin. Keep chart opaque.
The portrait must fit centered with 2 empty rows above and below, at least 3 empty columns left and right. Head only, no shoulders. Preserve BAMBI's distinct wider rounded face and cheeks, short neat dark hair swept to the side with slight temple recession, thick eyebrows, friendly small eyes, broad nose, closed-mouth smile and short chin/jaw beard. No large mustache. Compared with the reference, SIMPLIFY details into coarse whole-square clusters: hair 4-5 pixels tall, each eyebrow 4 cells, eyes 2-3 cells, nose 3 cells, smiling mouth 6 cells. Head footprint no larger than 26 wide by 28 high. No cells subdivided for pupils or outlines; omit a detail if it needs less than one cell. No curves, antialiasing, extra shades, rendered texture, gradients, fine marks, lettering, checkerboard or new guides. The result should look like a simple colored-in graph-paper mosaic, every feature aligned exactly to the existing cells.
```

## Coarse design

```text
Use case: stylized-concept.
Create a single coarse pixel-art FACE portrait of BAMBI for NICASOURCE JAM, in precisely the same low-resolution style as the approved MATUS portrait in Image 2. Image 1 is BAMBI's photographic likeness reference ONLY. Image 2 is the approved visual style and palette reference ONLY. Do not copy MATUS's face, spiky hair or large mustache onto BAMBI.
BAMBI identity cues from Image 1: broad round face, full rounded cheeks, short close-cropped dark hair brushed to one side, subtly receding temples, dark slightly arched eyebrows, small friendly eyes with smile creases, broad softly rounded nose, confident gentle closed-mouth smile, short dark chin beard following the lower jaw, mostly clean upper lip. Head and short neck only, no shirt or shoulders. Face nearly frontal.
Design as a genuine 32 by 32 logical-pixel face enlarged with perfectly sharp square blocks. The head including hair fits about 24 pixels wide by 27 pixels high centered in the canvas with margins on every side. Very coarse simplicity like Image 2: eyebrows about 4 pixels wide, eyes 2 to 3 pixels each, nose 3 pixels wide, smile 6 pixels wide, hair 4 pixels tall. Rounder and wider facial shape than MATUS, no tall hair spikes.
Only THREE opaque flat artwork colors: ink #22303A, skin shadow #B96D47, skin light #F2C18D. Empty background solid opaque magenta #FF00FF. Large flat connected clusters and no tiny detail. No visible grid, text, labels, UI, shirt, uniform, gradients, dithering, subpixel marks, antialiasing, realistic lighting, or detailed high-resolution portrait rendering.
```

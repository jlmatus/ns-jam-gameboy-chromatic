# MATUS — photo likeness revision v3

Integrated into the large player card **for user review**, not marked approved.
The user supplied `joe.jpeg` again because the current portrait did not resemble
him. The approved Night Court artwork and earlier MATUS versions are preserved.

## Current master

- `pixels.json`: explicitly authored **48×48** cells, four opaque colors plus
  empty space. Short swept hair, natural eyebrows/eyes, a slight closed smile,
  warm trimmed mustache/goatee and rounded cheeks follow the supplied photo.
- `matus-48.png`: transparent native master.
- `matus-preview.png`: exact 8× nearest-neighbor enlargement of those cells.
- `design-reference.png`: generated likeness study; it is not cartridge artwork.
- `prompts.md`: exact built-in Image Generation prompts. No API/CLI fallback.
- `generated-grid.png`, `blank-grid-48.png`, `grid-review/`: retained reconstruction
  attempt and raw diagnostics. It has 49 guides per axis, but **fails strict grid
  fidelity** (382 rejected cells, 333 requiring review). It is not accepted as an
  exact recovery. Native contours, eyes, hair, mouth/goatee and neck are rebuilt
  as whole-cell clusters in the editable master; no generated raster is shrunk
  into the game.
- `studies/`: earlier manual/compact attempts and the first generated study,
  retained as exploration rather than current assets.

## Integration

`art/ui/scouting-cards.json` uses `cardSource: "matus-v3"` for the large card.
Its existing `source: "matus-v2"` remains the compact legacy reference.
`scripts/create-front-proof.py` renders the 48px master at exact **2×** scale
inside the existing 96px card region, with separate card palettes. All four
selection stages use the same master. Other players, supplied ratings, body
sprites and the current intro animation are unchanged.

Regenerate with `python3 scripts/refresh-matus-portrait.py --source matus-v3`,
then run `scripts/create-front-proof.py` using Python with Pillow. Four opaque
colors are packed as background art through the custom native renderer, not a
standard three-color OBJ sprite. No native asset IDs were replaced.

See [actual cartridge evidence](../../../docs/playtests/matus-v3.md).

# MATUS — faithful card reconstruction v4

The user rejected v3 because the card's separate 48px redraw did not resemble
the generated portrait study shown alongside it. This revision uses that same
[portrait study](../matus-v3/design-reference.png) as its source, preserving the
hair silhouette/highlights, brows, eye positions, nose, lips, smile and goatee.
**Approved by the user on 2026-11-02.** Use this version as the MATUS likeness reference for future card and intro artwork.

- `pixels.json`: 96×96 indexed whole-cell reconstruction, source hash/bounds,
  palette, explicit native rows and preserved head aspect ratio.
- `matus-96.png`: native master with empty-space alpha.
- `matus-preview.png`: exact 6× enlargement; no smoothed preview.

Run `scripts/reconstruct-matus-card.py` using Python with NumPy/Pillow, then
`scripts/create-front-proof.py`. The first script extracts the connected head,
removes detached alpha/color debris and rebuilds flat source-color regions onto
the native grid. It does not invent a different face. The card compiler maps
these regions into seven legal four-color background palette families. No
dithering or filtered display scaling is used.

The head occupies 63×92 pixels in the 96×96 master. The game renders it **1:1**
within the existing card region, instead of magnifying the rejected 48px redraw.
Earlier portrait versions, original study and its exact image-generation
[prompts](../matus-v3/prompts.md) remain intact. No new Image Generation call
was used for this revision.

`art/ui/scouting-cards.json` points `cardSource` here. Its compact legacy
`source` remains separate. See [actual cartridge evidence](../../../docs/playtests/matus-v4.md).

# BAMBI detailed card portrait v2

A 96×96 reconstruction of the original `../bambi-v1/design-reference.png`
portrait study. The source study and compact v1 artwork are preserved.
Facial landmarks and aspect ratio follow the original study, with full-cell
palette regions replacing the lower-detail 32×32 card enlargement.

- `pixels.json`: editable 96×96 source, provenance hash, palette and cell rows.
- `bambi-96.png`: transparent native master.
- `bambi-preview.png`: exact 6× nearest-neighbor review image.

Reproduce with `scripts/reconstruct-roster-cards.py`; generate native card tiles
with `scripts/create-front-proof.py`. `cardSource` selects this large master;
`source` retains v1 for the compact portrait workflow. CHRUX additionally keeps
the source study's slate hair highlights in compatible tile palette families.

Integrated for review; likeness approval is pending. Original source photographs
were not located in this repository; the saved approved portrait study is the
reconstruction reference. No new likeness was invented with image generation.

# Side-facing Court Bodies

`players-front-before.png` and `pixels-before.json` preserve the preceding court
sprites. `facing-comparison.png` compares those frames (left) with the new profiles
(right), at exactly 2× nearest-cell scale. Rows follow roster order; columns show
idle, three walk/dribble frames, raised arms, then the mirrored direction.

The editable source is `scripts/create-prototype-art.mjs` and its generated
`art/prototype-pixels.json`. The existing 16×32 cells, 120 frames, native sprite ID,
palette indexes, and jerseys remain. Heads have one visible eye, a projecting
nose, and a side jaw. Raised hands reach toward the basket. Approved selection
portraits are untouched. Kits and animation remain provisional.

Regenerate with `node scripts/create-prototype-art.mjs`, then
`node scripts/configure-prototype.mjs` to refresh the checksum while preserving IDs.
The plugin analyzed 142 distinct 8×8 tiles, one source palette family, and no
source-pixel violations. Native ROM verification is documented in
`docs/playtests/gameplay-balance.md`.

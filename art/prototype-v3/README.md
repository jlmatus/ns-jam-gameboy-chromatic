# Athletic Body and Framing Study

User screenshots from NBA Jam '99 guide proportions, articulated running,
front/back turns, and shot stages. The reference images are not imported as game art.
Approved portrait and scouting-card pixels are unchanged.

`players-before.png`, `players-before.gbsres`, `pixels-before.json`,
`court-before.png`, and `generator-before.mjs` preserve the preceding pass.
`scripts/player-poses.mjs` is the editable whole-cell source for ten poses:
ready, two strides, toward-camera, away-camera, gather, set, release,
follow-through, and landing. Bodies use a fixed foot anchor on a 16×48 canvas;
the standing silhouette is about 40 pixels tall. Mirrored walk poses redraw
jersey numerals afterward to preserve readable numbers. Small numerals are
omitted during shots and landing to reserve the tile budget for motion.

`scripts/create-prototype-art.mjs` generates the sheet and the 400-pixel court.
`scripts/configure-prototype.mjs` preserves IDs while extending the sheet to
240 metasprites. The net occupies the native PLAYER slot; four body actors
share one crew load, followed by the ball and human marker. This prevents a
second crew allocation. Compiled loader usage is 178 tiles per reserved bank
span, below the UI boundary at 192.

`pose-study.png` is an early source preview, not cartridge evidence. Genuine
final ROM clips and verification are in `docs/playtests/athletic-motion/` and
`docs/playtests/athletic-motion.md`. Final visual acceptance is pending user review.

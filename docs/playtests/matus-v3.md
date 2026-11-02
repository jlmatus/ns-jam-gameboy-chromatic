# MATUS v3 photo-likeness card

Revised the large MATUS card from the user's re-supplied `joe.jpeg`. A separately
authored 48×48 master replaces the previous enlarged 32px likeness on all large
card stages. It adds a fourth opaque color for the trimmed mustache/goatee and
uses exact 2× cell placement in the same 96px card region. Likeness approval is
pending. No intro redesign or game-rule changes are included.

## Native validation

- Official cartridge build exits zero, valid GBC-only 4 MiB NSJAM ROM SHA-256
  `ca63f8311d08f199ab27664585ba6edcc0631d579b6e2d98b1586c6a8d49a968`.
  Existing SDCC optimizer/stock SFX warnings remain; no build errors.
- Plugin graphics analysis validates the actual packed 160×144 card: four
  opaque portrait colors across five packed palette families, no per-tile color violations. The custom encoder
  uses 222–223/256 pattern slots for the four stage-header variants.
- Genuine PyBoy 2.7.0 recorded session `c09388ac33894a0ea8b20e7dd4cbc56f`,
  `game/build/matus-v3-final`, branch `branch-0001`, finalized passed at frame 787.
  Boot/home, MATUS main card, MAGIC and back, unchanged MATUS rating page/back,
  MATUS CPU-teammate card, full lineup and fresh match entry were observed.
  The initial `game/build/matus-v3` recording remains retained; a final palette
  adjustment restores gold lettering on tiles shared with the portrait frame.
- All 23,040 pixels of the main card capture match the packed PNG after PyBoy's
  RGB555 expansion. The home capture is byte-identical to the approved Night
  Court v2 capture. Hash checks preserve v2 portrait files, other player cards,
  intro frame modules and shared intro tiles.
- Official source export and sprite check pass at 190/192; `git diff --check`
  passes. No new WRAM buffers or OBJ graphics were introduced.
- The existing browser listener still reports unresolved recording-close
  acknowledgement. Its current view has human playback active and old build
  identity. No conflicting refresh/rebuild was attempted. These captures verify
  the cartridge rather than a refreshed browser or physical Chromatic.

## Artwork provenance

Built-in Image Generation supplied likeness studies and a chart attempt. The
48×48 chart has 49 guides per axis but fails strict grid fidelity; raw results
remain in `art/portraits/matus-v3/grid-review/`. It is not accepted as exact
pixel recovery. The final native master explicitly reconstructs whole clusters
for the hair/contours, relaxed eyes, nose, smile, trimmed facial hair and neck.
See the [source notes and exact prompts](../../art/portraits/matus-v3/README.md).

## Captures

[Actual MATUS card at 4×](matus-v3/matus-4x.png),
[native card](matus-v3/matus.png), [teammate stage](matus-v3/matus-teammate.png),
[MAGIC palette regression](matus-v3/magic.png), [ratings](matus-v3/ratings.png),
[restored match graphics](matus-v3/match.png), [unchanged home](matus-v3/home.png),
and [final build/input/frame receipts](matus-v3-final-evidence.json).

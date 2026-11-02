# MATUS card fidelity correction

The user identified a mismatch between the shown portrait study and v3's in-game
face. Replaced the alternate 48px redraw with a 96px native reconstruction of
the actual study. The head keeps its source aspect ratio and feature positions;
hair highlights and lip/facial-hair colors now survive the native conversion.
Likeness approval remains pending.

## Validation

- Official build exits zero, valid GBC-only 4 MiB NSJAM ROM SHA-256
  `f1991756fd48f8b08e1880a3300ff78d3bc7c6ba644436e30fde4026fb83edd5`.
  Existing compiler warnings remain; no errors.
- Packed 160×144 card passes plugin analysis with seven palette families and
  no per-tile color violations. Stage variants use 227–229/256 pattern slots.
- Genuine PyBoy 2.7.0 recorded session `9c792bc557f24e38b066c82c3b1dd032`,
  `game/build/matus-v4`, branch `branch-0001`, finalized passed at frame 787.
  Main MATUS card, MAGIC/back, unchanged rating page/back, CPU-teammate MATUS,
  completed lineup and fresh match entry restore correctly.
- All 23,040 pixels of the card capture match the packed PNG after PyBoy's
  RGB555 expansion. Approved Night Court home capture is byte-identical to its
  prior capture. Hash checks preserve v3 files, other cards and intro frames.
- Official source export and sprite check pass at 190/192; diff whitespace
  checks pass. No simulation changes or additional WRAM buffers.
- Existing browser listener still reports unresolved recording-close status;
  its old-build human view was left running. No conflicting refresh was tried.
  Verification is of the actual cartridge, not an updated browser or hardware.

## Evidence

[Actual corrected card at 4×](matus-v4/matus-4x.png),
[native card](matus-v4/matus.png), [teammate](matus-v4/matus-teammate.png),
[MAGIC](matus-v4/magic.png), [ratings](matus-v4/ratings.png),
[match](matus-v4/match.png), [unchanged home](matus-v4/home.png), and
[build/input/frame receipts](matus-v4-evidence.json).

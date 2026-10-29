# Native Dunk Cam artwork

Editable finishing poses extend `scripts/player-poses.mjs` with gather, takeoff, cock-back, slam reach, rim hang and landing cells. Existing court poses and approved portrait sources are preserved. Height, body build, jersey and distinguishing head cues come from the existing roster art.

Run `node scripts/create-dunk-art.mjs` after changing those cells. It writes the banked native frame modules, the opaque camera HUD font, source PNG studies and `manifest.json`. The source previews are illustrations of the native frame data, not emulator evidence; actual captures are in `docs/playtests/dunks/`.

There are 32 normal frames and 24 failed-finishing frames per player, sampled every two simulation updates. Left-basket maps flip the scene while precompensating jersey digits. The green CHOMPA beanie receives its palette after the head's tile coverage is known. The generator rejects frames exceeding the reserved transition-tile budget.

During the covered portion of the camera, complete frames alternate background VRAM banks and tile maps, then publish at VBlank. Transition frames use the reserved window area. The camera uses the same foreground translation for the dunker, loose/blocked ball and contesting defender so short takeoffs cannot make the ball jump to a different origin. The renderer restores court data, palettes, the normal HUD and overlay settings on exit.

Visual acceptance remains with the user; passing tile checks alone does not establish finished animation quality.

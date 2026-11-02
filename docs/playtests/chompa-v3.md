# CHOMPA card detail restoration — 2026-11-02

The 32px portrait lost the eye shapes, tilted glasses and beard texture when enlarged. The main card now uses a 96px reconstruction of the existing approved study, rendered at 1:1. No new portrait design or gameplay tuning.

Official native ROM built successfully: `c155b5561d3ef3644849ebf8e11fddea978f64458762d4d2f6b2a2ec62ebb231`. Actual PyBoy 2.7.0 recording `158d9ce2ac9b4128be6d922509845031`, branch `branch-0001`, frames 450–1138. Checked CHOMPA in all four selection stages, both ratings pages and return, neighboring portraits, and the transition into a normal match (captured at 2:59). All 23,040 actual CHOMPA framebuffer pixels match the packed card after emulator RGB555 expansion. Genuine captures and input receipts are retained alongside this note.

Packed cards use 238–239 of 256 background patterns, seven available palette slots, and four colors per tile. Plugin graphics analysis found no violations. Source export allocation remains 190/192 sprite tiles. Approved home and MATUS card image hashes match their pre-change hashes.

The official web build succeeded (ROM `683755892fc8a8c721c85c70bd2fe1864d9c17d0cf98710534a407d21730b61c`), but preview refresh returned `Browser recording close acknowledgement is UNKNOWN`. The existing tab remains on the older build. No forced reload, process reset or journal change was used. Screenshots below are genuine native-emulator output, not browser verification.

Visual likeness remains subject to user approval; these checks establish native rendering and navigation, not subjective acceptance or physical hardware testing.

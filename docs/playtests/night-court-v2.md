# Night Court main-screen revision

Replaced the first proof's flat home artwork with an authored native reconstruction
of selected concept A: visible volcano and city lights, sunset, palms, street lamps,
reflective dark court, hoop, foreground ball and a broad ivory/gold NS JAM logo.
This pass changes the home image and its palettes; it does not redesign the intro.

## Validation

- Official native build succeeded. ROM SHA-256:
  `5d3334ba6c1fa1aac4c517ea0261d94ef520ed2114d9e7da74fb538ba08a73cd`.
  Valid GBC-only 4 MiB NSJAM cartridge. Existing compiler warnings remain.
- Genuine PyBoy 2.7.0 recording `game/build/night-court-v2`, session
  `9bfa9a4a2783443b8dc79d7e555fdd44`, branch `branch-0001`, finalized passed
  at frame 912. Native boot reaches home; all three menu selections, help/back,
  Play Game to MATUS card/back, replay and return retain the correct palettes.
- Plugin graphics analysis on `game/artifacts/front-art/night-court.png` reports
  valid 160×144 art, no tile-color violations and six distinct palette families.
  The custom pattern encoder uses 251–252/256 pattern slots for the three home
  variants. Signed BG indices 0–127 in both VRAM banks preserve OBJ and resident UI.
  All 23,040 captured home pixels match the packed source after PyBoy's RGB555
  expansion, including tiles loaded from both banks and horizontal/vertical flips.
- Sanitized host rules/front-flow tests pass. Official source export and sprite
  budget check pass at 190/192. `git diff --check` passes.
- Browser export built successfully (ROM SHA-256
  `99f86af61bd471ef1ef854762989c8b78474e1fe1adc99898fc2a247ae948f57`),
  but publication to a refreshed player returned **Browser recording close
  acknowledgement is UNKNOWN**. The older owned player remains paused with
  its state saved as `7a15209588253b07b94e7370a74b6b1dfb8f91af225c30bfb4817d3e8d0054e4`.
  Inspection showed an idle recorder; the public close tool still could not
  confirm completion. No server/process reset, browser reload or recording
  bypass was used. The new browser-visible screen remains unverified.
- Previous finalized front-delivery recording was reversibly archived using
  the plugin to release recording quota; its curated evidence remains intact.
  No physical-device test was performed.

## Captures

[Actual cartridge home at 4×](night-court-v2/home-4x.png),
[native 160×144 home](night-court-v2/home.png),
[help](night-court-v2/help.png), [MATUS card](night-court-v2/card.png),
[return from card](night-court-v2/home-return.png), and
[input/build/frame receipts](night-court-v2-evidence.json).

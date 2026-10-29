# Athletic Motion and Closer Framing

Responds to the user's NBA Jam '99 screenshots and rejection of upright bodies,
limited turning, proportions, and shooting. This is a new presentation candidate,
not a claim of final animation quality or reference parity.

## Changes

- Standing bodies grow from 32 to about 40 pixels, on 16×48 canvases. Heads are
  smaller relative to torso/legs. Strides bend knees and swing opposing arms.
- Front/back movement uses different torso/head poses; lateral movement uses
  profile strides. Jersey numbers remain readable after turning. Shots/landing
  omit tiny numerals to use their tiles for articulation.
- The logical court stays 320 pixels wide; its authored display projection is
  400 pixels. This is a fixed closer view, with 25% horizontal magnification and
  taller players, rather than browser scaling or continuous dynamic zoom.
  Camera framing includes the human and nearby ball where they fit.
- Holding B gathers the ball, bends the knees, then raises the hands/body.
  Release, follow-through, descent, and landing have distinct timing. CPU shots
  now have an interruptible 18-update windup. Blocks use a bounded jump arc.
  Ball flight begins at the release height; the rim is 44 pixels above its
  ground plane. Point classification still uses the shooter's feet.

## Verification

`sh scripts/test.sh` passes with warnings as errors, ASan, and UBSan. New checks
cover gradual takeoff, the ball rising with hands, release/landing, frozen
animation during pause, and interrupted CPU windup. Existing ratings, Warm-up
balance, 1,080 lineups, and multi-level soaks pass. Individual deterministic
soaks need not include every outcome; explicit rebound/scoring tests remain.

Final cartridge: 262,144 bytes, valid GBC-only header/logo. SHA-256:
`3f94ee236bd2d53caa49af5c460335cfc35105ab1d9a39ce4f9e5913ec5f4196`.
The official CLI build succeeded with Node DEP0190 and five SDCC optimizer
warnings. PyBoy retained final-ROM inputs/frames through frame 617, including
gather/rise/release/landing, a made human two, front/back/lateral movement,
and intact court/HUD. The score reached 2–0. An earlier pre-guard ROM also ran
through frame 1181 with a made three, CPU misses/rebounds/scoring, and a 3–2 score;
its captures are retained separately from the final cartridge's evidence.

Final genuine clips: `athletic-motion/shooting-close.gif` (frames 395–511,
1.96 seconds) and `athletic-motion/turning-final.gif` (543–617, 1.26 seconds).
The earlier `shooting.gif`/`turning.gif` belong to the pre-guard ROM. Clips use retained native
frames with original timing; sparse samples are held, never interpolated.
The final recording is passed; failed iterations remain recoverably archived.

Compiled crew payloads use 170 tiles per VRAM bank span. The actual loader uses
178 including net/ball/marker, below reserved UI tile 192. Run
`sh scripts/build.sh export` then `node scripts/check-native-sprite-budget.mjs`.
This catches duplicate scene/player loads, which corrupted the first larger
build. Explicit persistent ball screen coordinates also resolved invalid
positions in the larger rendering path. A close-shot regression exposed eleven
objects on four scanlines. A constant-size interval intersection now suppresses
only the marker when that would displace a body. The same frame 511 measures ten
objects and zero over-limit lines. Other final samples peaked at six to nine.
These samples do not establish worst-case or physical performance.

The separate official web export succeeded, SHA-256
`f2e21470603b309d343c96fbdcc698acf5f69b35135a95515e0deaf7c440bafb`.
Previous browser play was saved at frame 41432, state
`7de5c4a33c15af2834bba295e4ea55e56133add6fce2edb7913b4587f8b5fa51`,
before replacing the preview. The old service reported an unclean close after
that acknowledged save. An intermediate review screen was also saved at frame
20771, state `b3bb853d25e9c32517741062754345e4c719cdfbf552029fa0bb3d226db54a78`.
The final built-in browser check confirmed lineup selection, Warm-up, movement,
turning, and native pause; its checked console had no errors or warnings. It is
left paused in-game; P resumes. See `athletic-motion/browser-paused.png`.
Physical Chromatic testing remains unperformed.

Evidence: `athletic-motion-evidence.json`, clips/captures in `athletic-motion/`,
and editable/preserved sources in `art/prototype-v3/`.

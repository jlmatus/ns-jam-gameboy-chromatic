# Signature dunks — 2026-11-04

## Problem

All six players did the same tomahawk in Dunk Cam. Only the jump height (from the dunk rating) and a small two-hand grip for BAMBI and CHOMPA changed ([before](dunk-styles/current-dunks.png)).

## Change

`scripts/dunk-poses.mjs` now holds one keyframe set per player (`dunkStyles`). Each dunk is matched to the player's card tagline:

| Player | Tagline | Dunk |
|---|---|---|
| MATUS | TOP OF THE KEY | Cradle jam: the ball swings low and back, then up for a one-hand flush |
| MAGIC | OVER THE TOP | Windmill: the straight arm circles front, down, back and over |
| BAMBI | BULLDOZER | Two-hand power slam: tucked knees, double shake, rim rattle and landing dust |
| CHRUX | BACK DOOR | Reverse jam: drifts under the rim and slams back over his head |
| CHOMPA | STRETCH BIG | Two-hand rim hang: stays on the rim until update 44, swinging his legs |
| MEAT | BIG ENERGY | 360 spin: front, profile, back and mirrored-profile heads, with motion arcs |

Other details:

- **Two-hand dunks** (BAMBI, CHRUX, CHOMPA) carry the ball between both wrists. Each hand anchor follows the arm, so the live ball OBJ stays in the hands.
- **Side and back views.** Profile and back heads reuse the court sprite heads, and profile frames hide the chest number.
- **Generator.** `scripts/create-dunk-art.mjs` reads each style's reach, hang length and impact strength. It writes previews to `art/dunks-v3/`; v1 and v2 are preserved.
- **Rules unchanged.** Every dunk keeps the shared 64-update timeline: the ball reaches the rim at update 28, blocks happen in the same window, and scoring is unchanged. No C rules code changed.

Source previews (ball drawn approximately; not emulator frames):

- [MATUS](dunk-styles/matus-strip.png)
- [MAGIC](dunk-styles/magic-strip.png)
- [BAMBI](dunk-styles/bambi-strip.png)
- [CHRUX](dunk-styles/chrux-strip.png)
- [CHOMPA](dunk-styles/chompa-strip.png)
- [MEAT](dunk-styles/meat-strip.png)

## Validation

- **Tile budget.** All 504 generated frames fit: the busiest uses 124 of the 128 cinematic tiles.
- **ROM build.** The official CLI build passes, SHA-256 `0fc2c0d823be4b1b26b68006c948105ea29915df74f12756351b73053a744cb4`.
- **Sprites.** After an export, the sprite budget check passes at 182/192.
- **Rules tests.** `sh scripts/test.sh` passes.
- **Emulator play.** The exact ROM was played in PyBoy with [`dunk-styles/play.py`](dunk-styles/play.py), which selects each player in the menu and drives to the rim. Inputs and RAM logs are in `dunk-styles/play/run-*.json`.

  | Run | Result |
  |---|---|
  | MAGIC | Made after four failed attempts |
  | BAMBI | Made after one failed attempt |
  | CHRUX | Made |
  | CHOMPA | Made |
  | MEAT | Made |
  | MATUS | Four attempts, none made (dunk rating 3); [blocked](dunk-styles/play/matus-failed-1-strip.png) cradle frames shown |
  | CPU CHRUX | [Made at the left basket](dunk-styles/play/cpu/chrux-left-made-strip.png); the mirrored reverse jam and the name read correctly |

- **CHOMPA's hang.** He stays on the rim while the ball drops through the net, then the game returns to the court at 2:14 ([finish](dunk-styles/play/chompa-finish-strip.png)).

Each run's GIFs are in `dunk-styles/play/`.

## Not done

- No MATUS make was captured in the emulator; only the source preview shows his made finish.
- The browser and physical Chromatic are unverified.
- Visual approval of the six moves is still with the user.

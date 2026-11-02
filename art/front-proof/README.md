# NS JAM native presentation proof

Hand-authored, deterministic 160×144 pixel composition. No high-resolution concept image or photo is imported or downsampled. `scripts/create-front-proof.py` draws the Night Court home, help, four MATUS stage headers and twelve MATUS shooting poses. Run it with Python 3 plus Pillow; the desktop bundled Python includes Pillow.

Native PNGs are exact packed RGB555 output. `-4x.png` files are nearest-neighbor source previews, not emulator captures. Actual ROM captures and the input-derived GIF are in `docs/playtests/front-proof/`.

- Home has three highlighted states: Play Game, How to Play and Replay Intro.
- MATUS uses the existing `matus-v2/pixels.json` face at exact 3× scale (96×96), preserving cell geometry. Card source and other five portraits remain untouched. Ratings remain speed 7, three-point 10, passing 10 and steal 8.
- The proof contains one roughly 4.5-second MATUS shot with twelve authored poses, airborne feet, visible ball arc, rim/net passage and existing shot/swish cues. The full five-player intro and music theme remain later work.
- Every 8×8 tile has at most four colors; seven background palettes are used. The generator chooses tile-local palette fits and records palette approximation error in `manifest.json`; final packed pixels are the reviewable output.
- Individual pages use 166–227 background tiles. Intro rendering shares 134 immutable patterns and two disjoint 53-tile pose regions: 240/256 patterns across signed BG tile indices 0–127 in both VRAM banks. Hidden tilemaps swap at VBlank. Fonts/gauges 192–236 and OBJ patterns are untouched.
- Full-screen page changes use a short uniform navy palette stage during upload, avoiding partial images and LCD-off white frames.

Generated banked C lives in `game/plugins/basketball/engine/src/core/jam_front_*`. These source studies are ready for visual review, with native build/play evidence recorded separately.

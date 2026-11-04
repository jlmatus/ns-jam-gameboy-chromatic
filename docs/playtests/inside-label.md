# DUNK rating renamed INSIDE — 2026-11-04

The rating that drives dunks and every shot inside the arc is now labeled **INSIDE** on the scouting cards:

- main player cards for MAGIC, BAMBI, CHRUX and MEAT, which feature it;
- the "RATINGS 1 OF 2" page for all six players.

The value, the formulas and the C field name (`dunk`) are unchanged. "TURBO B DUNK NEAR RIM" on the help screen still names the move, not the rating.

## Changes

- **Card copy:** `art/ui/scouting-cards.json` and `scripts/create-scouting-cards.mjs` regenerate the `jam_card_*` rating pages.
- **Main cards:** `scripts/create-front-proof.py` regenerates the main player cards (`jam_front_frame_4`–`39`).
- **Right-aligned values:** main-card values are now right-aligned, as on the ratings pages. INSIDE plus a two-digit value would not fit the 53-pixel column, so single digits moved one character right, leaving a gap after INSIDE. This moves single-digit values on all six main cards.
- **Unchanged:** the approved home and help frames (0–3), the intro frames and all portrait pixels come out identical.

## Validation

- The CLI ROM build passes.
- Rules tests pass.
- The sprite budget check passes at 182/192.
- Card tile counts stayed the same or dropped by one.
- [Emulator capture of all six cards and rating pages](inside-label/cards.png), made with `inside-label/capture.py` on the exact ROM.

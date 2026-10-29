# MATUS — portrait revision v2

Revises the side-compressed face reported on the player card. The eyes now sit on one line with balanced spacing; the cheeks and jaw have a fuller front-facing silhouette. Short dark hair, warm skin, moustache and compact goatee follow the original `joe.jpeg` reference. No uniform or extra colors were added.

The previous approved [v1](../matus-v1/README.md) remains intact. This revision is integrated for review, not marked as user approved.

- `pixels.json`: authoritative, deliberately placed 32×32 cells and the original three opaque colors.
- `matus-32.png`: transparent native-size portrait.
- `matus-preview.png`: exact 12× nearest-neighbor source preview.
- `matus-native-source.png`: identical cells mapped to native import shades.

Run `python3 scripts/refresh-matus-portrait.py`, then `node scripts/create-scouting-cards.mjs` to rebuild the image files and native cards. The card enlarges each source cell to exactly 2×2 display pixels; it does not stretch the portrait horizontally.

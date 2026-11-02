# Night Court home artwork — revision 2

**Approved and locked by the user on 2026-11-02.** Preserve this main-screen
composition, logo and palette during intro work. Change its visual design only
when the user requests it. This approval applies to the home artwork; the intro
animation remains pending redesign.

Reconstructs approved home concept A with a broad volcano behind the lit city,
sunset bands, framing palms/street lights, chain-link fence, dark reflective court,
hoop and foreground basketball. The display name is NS JAM.

Editable native-resolution clusters and the custom block logo live in
`scripts/night_court_art.py`. Run `scripts/create-front-proof.py` using Python
with Pillow to regenerate the packed PNGs and banked cartridge data. No concept
sheet pixels are sampled. The concept remains a composition reference.

`home.png` is the exact packed 160×144 source; `home-4x.png` enlarges those same
pixels with nearest-neighbor scaling. Genuine cartridge captures are in
`docs/playtests/night-court-v2/`. The earlier home capture remains in
`docs/playtests/front-proof/home.png` for comparison.

Home has its own seven palette slots (six distinct families), independent of the
intro/card palettes. Each tile uses four colors or fewer. The selected menu
variants use 251–252 pattern slots, including mirrored reuse, within the 256
slots reserved for front art. The twelve-pose intro and player cards are unchanged.

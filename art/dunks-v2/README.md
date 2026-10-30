# Animated arcade finishing study

`scripts/dunk-poses.mjs` authors articulated 24×56 native cels with crouch, takeoff, cock-back, extension, two-hand grip, leg swing, landing and recovery keyframes. All six confirmed body profiles and canonical face/jersey cues remain in use. BAMBI and CHOMPA use a two-hand power gather; the other players use a one-hand tomahawk gather. These are presentation styles, not new ratings or abilities.

`node scripts/create-dunk-art.mjs` writes 504 banked frames, this manifest and source PNG studies. The earlier v1 studies remain preserved. Each cel is authored through integer joints and filled cells; source previews use nearest-cell zoom. They omit the live OBJ ball and are not emulator captures.

The arena animates speed streaks, a small impact shake, expanding rays, rim rattle, net stretch/sway, cheering silhouettes and landing dust. Failed finishes omit made-basket impact effects. Camera frames carry explicit hand anchors, and native OAM is published before the same VBlank as the new tilemap so the ball follows fast arm motion.

Large 3×2-cell player-name lettering now fills the upper arena behind the athlete. Foreground tiles mask the word so skin, kits and basket colors stay intact. The left-basket canvas pre-mirrors only the name, preserving readable lettering after the camera turns. The name is omitted during the court wipes so resident HUD/gauge tiles remain protected.

Current maximum is 123 unique tiles/frame within the 128-tile cinematic page; transition frames use at most 67 of their 83-tile limit, reserving bank-zero tiles 232–236 for the turbo gauge alongside the font. Earlier motion verification is in `docs/playtests/finishing-v2.md`; the name-background revision is in `docs/playtests/dunk-names.md`. Visual acceptance remains with the user.

Rejected dunks now have a separate blocked variant: a bouncing BLOCKED stamp with opaque gold/ivory panel, precompensated for the left basket. It uses at most 117 of the 128 background tiles, stays clear of OBJ-overlap indices 128–191, and adds no sprites. Native sounds accompany takeoff, slam and rejection. See `docs/design/audio.md` and `docs/playtests/blocked-audio.md` for source recipes, actual-ROM PCM and captures.

# Arcade finishing and Dunk Cam

## Controls and eligibility

Hold START for turbo and press B near the attacking basket. A dunk costs 12 turbo units and requires an available reserve. B without turbo, outside the dunk area or without enough reserve keeps the charged jump-shot behavior. The takeoff must be on the court-facing side of the rim and within 18 court pixels of its depth lane. A stunned player cannot dunk. See [turbo rules](turbo.md).

Takeoff range is `10 + 4 × dunk rating` court pixels. Open finishing chance is `42 + 5 × dunk + floor(power / 2)` percent, reduced by nearby defensive blocking pressure. Opponents receive difficulty adjustments and ceilings of 70/90/95 percent; the supplied ratings stay unchanged. CPUs with finishing-oriented ratings drive closer and can choose the same dunk action after a short gather.

## Simulation

`jam_dunk.c` owns the separate `JAM_DUNK` mode. Its 64 updates cover gather, takeoff, exaggerated rise, downward finish, optional rim hang, landing and return. Peak jump is `30 + 2 × dunk rating` pixels, compared with the ordinary 14-pixel jump. The confirmed height/build profiles remain in use.

The shooter travels from the actual launch position toward the rim. Ball coordinates remain attached to the hands until the finish or a block. B can time a nearby human block; CPU defenders telegraph one contest. A block requires both horizontal proximity and sufficient height/jump/reach. A successful contest shows the defender in the close-up. A miss or block returns a loose ball for a rebound. A make awards exactly two points at update 34, after the ball passes below the rim, then enters the existing retrieval/baseline-inbound sequence.

Other players and the possession timer stay still during the cinematic. SELECT freezes the dunk, ball, result and random state. A/B cannot interrupt it into a pass or another shot. The existing paused B action returns to the picker.

## Native rendering

The close-up uses deliberately authored 24×56 crew cels with crouch, cock-back, scissoring legs, reach, swinging rim hang and landing. `scripts/dunk-poses.mjs` owns the joint keyframes; `scripts/create-dunk-art.mjs` generates banked data and the manifest in `art/dunks-v2/` (v1 studies are preserved). BAMBI/CHOMPA use two-hand power gathers; others use one-hand tomahawk gathers. Zoom and framing follow the cinematic phase, with entry/exit wipes. Left-basket maps turn the action while precompensating jersey digits.

During the covered camera view, large muted teal arena lettering spells the dunker's name above and behind the action. Foreground-tile masks keep lettering out of skin/kit/basket palette cells. The left-basket canvas pre-mirrors the lettering before the whole camera flip so all six names remain readable; the HUD name remains available. Lettering is omitted from entry/exit wipes to preserve the font and turbo-gauge reservation.

During the covered portion, the renderer alternates background tile banks and tile maps, publishing each complete frame at VBlank. Fonts remain separate. The transition frames use the reserved window area; the renderer reloads the court tiles and repaints the court before revealing it. It preserves engine time instead of calling the stock scroll reset that would clear it. The live ball uses an enlarged native OBJ, and the stock overlay sprite-hiding flag is restored on exit.

While held, the live ball follows each cel's explicit hand anchor. Once released or blocked, the camera translates its simulation position relative to the staged dunker, as it does for the defender. Camera publication renders/activates OAM before the same VBlank as the new tilemap, preventing the fast arm swing from outrunning the ball. The HUD uses opaque priority tiles to mask sprites below the playfield. Made-basket impact adds restrained shake, expanding rays, rim rattle, net whip and crowd motion; landing has dust strokes. Failures omit the made-basket impact effects.

Blocked dunks use a distinct animated BLOCKED stamp and native rejection cue; made dunks use takeoff/slam cues. Misses remain separate. See [audio and blocked presentation](audio.md).

These are cinematic close-up compositions, not a continuous hardware scale transform of the court. Actual possession, score and world positions stay in the simulation. The turbo gauge remains visible and frozen during the sequence; fire streaks, sound and full-match clocks remain separate features.

# Arcade finishing and Dunk Cam

## Controls and eligibility

Hold START for turbo and press B near the attacking basket. B without turbo, or outside the dunk area, keeps the charged jump-shot behavior. The takeoff must be on the court-facing side of the rim and within 18 court pixels of its depth lane. A stunned player cannot dunk.

Takeoff range is `10 + 4 × dunk rating` court pixels. Open finishing chance is `42 + 5 × dunk + floor(power / 2)` percent, reduced by nearby defensive blocking pressure. Opponents receive difficulty adjustments and ceilings of 70/90/95 percent; the supplied ratings stay unchanged. CPUs with finishing-oriented ratings drive closer and can choose the same dunk action after a short gather.

## Simulation

`jam_dunk.c` owns the separate `JAM_DUNK` mode. Its 64 updates cover gather, takeoff, exaggerated rise, downward finish, optional rim hang, landing and return. Peak jump is `30 + 2 × dunk rating` pixels, compared with the ordinary 14-pixel jump. The confirmed height/build profiles remain in use.

The shooter travels from the actual launch position toward the rim. Ball coordinates remain attached to the hands until the finish or a block. B can time a nearby human block; CPU defenders telegraph one contest. A block requires both horizontal proximity and sufficient height/jump/reach. A successful contest shows the defender in the close-up. A miss or block returns a loose ball for a rebound. A make awards exactly two points at update 34, after the ball passes below the rim, then enters the existing retrieval/baseline-inbound sequence.

Other players and the possession timer stay still during the cinematic. SELECT freezes the dunk, ball, result and random state. A/B cannot interrupt it into a pass or another shot. The existing paused B action returns to the picker.

## Native rendering

The close-up uses deliberately authored crew poses with gather, cock-back, reach, rim-hang and landing frames. `scripts/create-dunk-art.mjs` generates banked tile data and a source/timing manifest in `art/dunks-v1/`. Zoom and framing follow the cinematic phase, with a short entry/exit wipe. Left-basket maps turn the action while precompensating jersey digits so numbers stay readable.

During the covered portion, the renderer alternates background tile banks and tile maps, publishing each complete frame at VBlank. Fonts remain separate. The transition frames use the reserved window area; the renderer reloads the court tiles and repaints the court before revealing it. It preserves engine time instead of calling the stock scroll reset that would clear it. The live ball uses an enlarged native OBJ, and the stock overlay sprite-hiding flag is restored on exit.

The camera applies one foreground translation to the ball and defender relative to the staged dunker, preserving their relative position even for short takeoffs. The HUD uses an opaque font and background priority to mask sprites below the playfield.

These are cinematic close-up compositions, not a continuous hardware scale transform of the court. Actual possession, score and world positions stay in the simulation. The final turbo meter, fire streaks, sound and full-match clocks remain separate features.

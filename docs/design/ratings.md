# Roster, Selection, and Rating Effects

The canonical supplied roster is in `jam_sim.h`. Names, signatures, positions, and all eight ratings retain the approved values. Jerseys are strings, including CHOMPA `74` and MEAT `00`.

## Selection

The native window screen runs before practice: choose the human, CPU teammate, two distinct opponents, and CPU difficulty, then review the lineup. Direction presses cycle available choices; A confirms and B returns. A held button cannot confirm twice. A or START launches the reviewed lineup. During practice, SELECT pauses; B while paused returns to lineup selection. Starting a reviewed lineup begins a fresh practice score.

The initial player occupies home slot 0 and the teammate slot 1. `controlled` identifies which home player receives human input. A successful home catch, rebound, or steal transfers control to the ball carrier; the other home player becomes CPU-controlled. Control stays with the passer while the pass travels, then moves on the catch. Global shot charge is cleared on handoff. Opponent possession does not transfer control to an opponent. Each player can appear only once.

The selected scouting-card layout uses 64×64 portraits made by exact 2× placement
of approved 32×32 cells, double-size player names, bold text, and ten-step meters.
SELECT cycles the main card and two four-rating detail pages. B closes details
before backing up; A confirms from any page. The six approved playstyle labels
are explained in [playstyle analysis](playstyles.md); they are not numeric ratings.

`art/ui/scouting-cards.json` controls card copy and featured skills.
`node scripts/create-scouting-cards.mjs` verifies values against the C roster and
creates native banked tile data. The menu reserves tile IDs 192–255 in both VRAM
banks. It uses background palette 5 for text/meters and 6/4/3 for approved portraits,
leaving UI palette 7 intact. Original portrait pixels remain unchanged. See
[layout verification](../playtests/scouting-card-layout.md).

## Current Rating Formulas

Positions and movement use quarter pixels; probabilities use integer percentages.

| Rating | Current effect |
| --- | --- |
| Speed | Movement is `3 + speed / 3` quarter pixels per update; holding turbo adds 2. Diagonal movement uses three quarters of that speed. |
| Three-point | Base chance is `12 + 3 × rating + floor(3 × rating² / 10)`. Subtract one point per three horizontal court pixels beyond 100. A specialist separates from an average shooter; deep heaves lose accuracy. |
| Passing | Flight lasts `28 − passing` updates. Ratings below 8 introduce up to `(10 − passing) / 2` pixels of vertical error. Higher passing also reduces interception reach. |
| Steal | Increases steal/interception reach and success chance. Human A reaches repeat at a rating-based cooldown; front positioning adds 20 chance points, reaching from behind subtracts 17. A carrier's passing/power protect the ball. Pass interceptions are probabilistic, with one attempt per defender per pass. |
| Block | Increases jump duration, blocking reach, and success chance (`10 + 7 × rating`). Each defender gets at most one block roll per shot. CPUs attempt nearby early-shot blocks. |
| Dunk | Ordinary close shots use `60 + 3 × rating` within 48 pixels; `52 + 2 × rating` for other two-pointers. Actual turbo dunks use takeoff range `10 + 4 × dunk`, peak jump `30 + 2 × dunk`, and base finishing chance `42 + 5 × dunk + power / 2`, before pressure/difficulty. See [dunks](dunks.md). |
| Power | Combines with build mass for gradual contact, shove resistance, and turbo runovers; reduces shooting pressure and protects possession. |
| Clutch | In a game within four points, after 720 simulation updates of possession, adds `2 × (clutch − 5)` to shot chance. Quarter-based clutch conditions can follow when clocks exist. |

A well-timed human charged release adds 10 points to shot chance; nearby defenders reduce it. CPUs gather for 18 simulation updates before releasing, without an automatic timing bonus. Human/team shot probability is bounded to 5–95%; opponent ceilings are 58%, 80%, and 92% by difficulty. Supplied ratings remain unchanged. Dunk rating also governs actual turbo dunk range, jump height and finishing probability; Dunk Cam follows the resulting simulation, including blocks, missed finishes and inbounds. The latest shot presentation raises the ball with the hands, animates a bounded jump, and preserves timing through pause; see [motion verification](../playtests/athletic-motion.md).

All shooters now use rejection-sampled percentage rolls. The previous human `byte % 100` roll favored makes: a nominal 52% attempt actually made about 61% before blocks. At 90 pixels from the rim, a timed open three now has a 44% chance for MAGIC, 65% for CHOMPA, and 82% for MATUS. These are shot-selection probabilities, not guaranteed results; blocks, pressure and distance can reduce actual makes. Warm-up changes opponent pressure, not the controlled player’s rating.

## CPU Levels

Difficulty changes opponents; the human and CPU teammate retain their ratings.

- **Warm-up:** opponent movement −1 quarter pixel, shot/steal/block modifier −8 points, shooting capped at 58%, defensive decisions every 16 updates, 20-pixel guarding gap, and 120-update steal cooldown. Opponent swipe chance is divided by three. Interceptions use reduced reach and half probability, with protected release/arrival frames. Human/team catches receive 90 updates of steal protection.
- **Pickup:** baseline movement/probability modifiers, shooting capped at 80%, defensive decisions every 8 updates, 16-pixel guarding gap, and 90-update steal cooldown. Opponent swipe chance is two thirds of baseline. Human/team catches receive 60 updates of protection.
- **Winners Stay:** movement +1 quarter pixel and probability modifiers +8 points, shooting capped at 92%, defensive decisions every 4 updates, 12-pixel guarding gap, and 60-update steal cooldown. Human/team catches receive 60 updates of protection.

Opponent swipe reach is `11 + steal / 2` pixels. Human reach is `(56 + 4 × steal + height_px − 40) / 4` pixels across court, with a separate depth check. Each defender gets one probabilistic interception attempt per pass, rather than automatically taking every ball within reach. An intended receiver within catch range wins the arrival catch; a failed lane attempt does not become a second automatic steal. Opponent catches have 45 updates of CPU-swipe protection, and initial practice possession has 120. Made baskets now use a separate protected inbound state and pass. A deliberate human reach can contest after the first eight possession updates; the old global protection no longer blocks manual defense for the entire CPU possession. These are prototype tuning values; easy-mode human shooting is preserved.

Difficulty never changes roster values. Signature text remains flavor. Four-quarter match flow, turbo meter, fire streaks, final uniforms, sound, and cartridge records remain later work. Exaggerated dunks and Dunk Cam are implemented.

## Physical Profiles and Contact

Confirmed heights/builds live in `art/roster-physics.json`; regenerate with
`node scripts/create-body-profiles.mjs`. Standing heights are 36, 42, 38, 38, 41,
and 38 native pixels. Lean/build mass classes are 1/3/4 (lean, stocky or broad,
heavy); unknown weights stay zero. Mass is a game tuning value.

Body radius depends on build. `power + mass` determines contact resistance;
a planted defender gains two resistance points. An offensive turbo impact adds
`speed / 4` and must beat resistance by three to run over a defender. Targets
recover after 18 updates; contact cooldowns prevent repeated knockdowns.
An outmatched charge can recoil for six updates. Ordinary overlap resolves by
at most one pixel per pair/update, replacing the old eight-pixel displacement.

On defense, START+A shoves a nearby opponent if attacker strength plus two meets
resistance. A successful carrier shove creates a loose ball and 16-update
recovery; stunned players cannot immediately collect, intercept, pass, or shoot.
A 40-update shove cooldown prevents repeated attempts every frame. The SHOVE
message survives the immediate loose-ball pickup so the result remains readable.

CPU diagonal normalization matches the human. Retreating defenders move at
three quarters speed and face the carrier. Supplied speed ratings are preserved;
height/build do not automatically reduce movement speed.

## Visible Baseline Inbounds

After a made basket, the receiving team's first slot retrieves the falling ball
from its actual position. All players walk into restart positions; no body jumps
to a new formation. The inbounder carries the ball beyond x=14/306, outside the
painted baseline, while the receiver takes x=64/256 and defenders keep a gap.
A passes the ball in once ready. CPUs wait 36 updates; idle home inbounds use a
180-update fallback. No movement, shooting, swiping, or possession timeout runs
during the dead ball. The inbound pass cannot be intercepted; its receiver still
gets normal catch protection. The inbounder returns onto court one pixel per
update during the throw. Pause freezes the sequence. Full foul, out-of-bounds,
and inbound-clock rules remain future match work.

The HUD always shows the controlled player's full name and jersey. The marker
shows their number, and pass feedback names the target. Player palettes separate
skin from white/red kits; CHOMPA keeps a green beanie and the approved scouting
portraits are unchanged.

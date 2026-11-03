# Basketball playtest — November 3, 2026

## Coverage

Three additional match attempts on the same ROM as the [earlier playtest](2026-11-03-issues.md), SHA-256 `d47273bc6923ec477c3e8fbcb27587974ca376b3162ef9987543382d28768b4a`, in PyBoy 2.7.0. All images are genuine 160×144 frames. The input files contain the complete held-button sets and durations, beginning after 120 clean-boot frames.

| Attempt | Lineup (human, teammate vs opponents) | Setting | Observed extent |
| --- | --- | --- | --- |
| [Warm-up](2026-11-03-basketball-review/warmup-inputs.json) | MAGIC, BAMBI vs CHRUX, CHOMPA | 1 period | Pass, control handoff, a make, steal, a second make from beside the basket, inbound; stopped at 2:40. |
| [Winners Stay](2026-11-03-basketball-review/winners-inputs.json) | MEAT, MAGIC vs BAMBI, CHRUX | 2 periods | Human shot and CPU action; display corruption appeared during Q1, stopped at frame 8219. |
| [Pickup](2026-11-03-basketball-review/pickup-inputs.json) | CHOMPA, MAGIC vs BAMBI, CHRUX | 4 periods | Human drive, blocked dunk and CPU play; HUD corruption appeared during Q1, stopped at frame 8389. |

This covers all three difficulty selections and all three length selections, but only three of the 3,240 ordered four-player lineup/difficulty/length combinations, plus one earlier combination. No complete additional game, quarter transition, overtime, final box score, or combination-wide result was verified. Length choices were confirmed in lineup review, not played to their endpoints. The available likeness references are stylized approved portraits, not photographs of the players.

## Findings

### High — HUD corruption recurs with different lineups and settings

The previous report's cinematic/court corruption is not limited to the original lineup. The [Winners Stay attempt at frame 4169](2026-11-03-basketball-review/winners-corruption.png) showed a severely corrupted screen; later court frames had garbled HUD text. The [Pickup attempt at frame 5389](2026-11-03-basketball-review/pickup-corruption.png) also had an unreadable HUD after cinematic play. These independent attempts strengthen the earlier finding; they do not prove a common cause. The damaged score/clock prevents meaningful rules and match-result checks.

### Medium — blocked Dunk Cam hides the action needed to understand the block

In the Pickup attempt, drive CHOMPA right, move down into the rim lane, then press START+B. At [frame 909](2026-11-03-basketball-review/chompa-dunk.png) the dunk begins; by [frame 969](2026-11-03-basketball-review/blocked-obscured.png) the large BLOCKED banner covers the center of the player, with no clear view of a defender making contact. The result is readable but the motion and cause of the block are hard to follow. The banner continues over the player for multiple sampled frames, then the [player reappears](2026-11-03-basketball-review/chompa-dunk-end.png). Consider showing the contest before the stamp or moving/reducing it. This is a presentation/readability finding, not evidence that the block calculation is wrong.

### Medium — player identity is difficult to recognize during normal court action

Compare [CHOMPA's card](2026-11-03-basketball-review/chompa-card.png) with the [court view](2026-11-03-basketball-review/chompa-court.png): the approved green beanie is a useful cue but the small on-court head does not convey the portrait's glasses, facial expression, or stubble clearly at native resolution. [MEAT's court view](2026-11-03-basketball-review/meat-court.png) likewise relies on the marker and HUD for identification. This is a visual readability assessment against the game's own card art, not a claim about resemblance to the real people. In [clustered contact](2026-11-03-basketball-review/cluster-contact.png), overlapping bodies further compete with the ball and markers. Review at actual handheld size and preserve approved portrait masters when improving sprites.

## Rules, ball and motion observations needing follow-up

- In Warm-up, a pass visibly transferred control to BAMBI, a normal shot scored two, and CPU inbound followed; a later steal and second basket also occurred. These samples are not enough to validate all scoring, possession, rebound, or stat rules.
- The second basket was released from the far side of the rim, close to the backboard: [setup](2026-11-03-basketball-review/behind-backboard-setup.png), [flight](2026-11-03-basketball-review/behind-backboard-flight.png), [score](2026-11-03-basketball-review/behind-backboard-score.png). On closer inspection, the available frames do **not** establish that the ball crossed the backboard or that the shooter was out of bounds. This corrects the preliminary playtest commentary; no rules or collision defect is confirmed from this sequence.
- Full foul and out-of-bounds rules are explicitly deferred in `docs/design/ratings.md`; their absence should be tracked as unfinished scope rather than presented as a newly discovered regression. No exhaustive check of collisions, shot trajectories, travel, blocks, steals, or rebound attribution was completed here.
- In two neutral-input intervals, the controlled player held the ball while the match clock ran. This alone does not show a CPU stall because control had switched to that player. The source contains a possession timeout, but a visible countdown and the turnover path were not verified in these runs.

Audio, physical hardware, all player likenesses in close-up, and any unlisted combination remain unverified. Original recordings remain under `game/artifacts/playtests/` in sessions `819b7d7c-6934-4f84-a9b6-45130483efdc`, `55a242ed-8dd3-4956-bf1d-2a1a58f1ed25`, and `9591b004-1f00-44e4-a79b-bc3c37646b9d`. No game code was changed.

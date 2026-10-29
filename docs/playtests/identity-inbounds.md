# Player Identity, Control Handoff, and Inbounds

Verified October 29, 2026 (Managua). The user requested recognizable players,
control of a teammate after passing, and an inbound after a made basket.

## Player Identity

Distinct eight-pixel heads retain the approved visual cues: MATUS spikes/goatee,
MAGIC glasses/clean jaw, BAMBI round face/beard, TORRES cropped hair/full beard,
CHOMPA green beanie/glasses, and MEAT's approved invented short-haired look.
Confirmed heights/builds remain unchanged. Skin and team-kit colors are separated
in the hardware palette; the previous kit colors obscured faces. White/red kits
remain temporary. Approved 32×32 scouting portraits are untouched.

The controlled player's marker now shows their jersey, including MEAT's `00`.
The HUD always shows their full name and number; pass feedback names the receiver.
The [enlarged identity guide](identity-inbounds/player-identity-guide.png) shows
actual authored standing cells with gameplay colors. It is a source-art guide,
not an emulator capture. Pre-edit cells remain in `art/prototype-v4/`.

## Control and Restart Behavior

Home slots 0 and 1 retain their identities and statistics. `controlled` selects
which receives human input; the other runs CPU logic. Control stays with the
passer during flight, then transfers to a home catcher, rebounder, or stealer.
Movement, charge/release, block/reach/shove, HUD, marker, and camera use that slot.
Shot charge clears on handoff. Holding A cannot immediately pass a catch away.
Enemy possession never transfers human control to an enemy.

After a make, `JAM_INBOUND` retains all actual player positions. The receiving
team's first slot retrieves the falling ball, carries it outside the painted
baseline, and waits for the other players to walk into restart positions.
Defenders keep space from the receiver. A/Z passes in when the home inbounder is
ready; CPUs throw automatically after 36 updates. A 180-update idle home fallback
keeps practice moving. The throw is protected from interception. The passer walks
back inside court bounds during flight, and control follows the home catch.
Pause freezes the entire sequence. Full foul/out-of-bounds rules remain later work.

## Validation

`sh scripts/test.sh` passes with ASan, UBSan, and warnings as errors. New checks
cover catch-based movement/shooting, held-A handoff, charge clearing, both restart
directions, no position teleport, manual/CPU throws, protected flight, return to
court, pause, and control/ownership invariants. The 108,000-update mixed-input soak
passes for all six initial players and all three CPU levels. Longer scripted
practice checks produce baskets/rebounds at each level.

The official CLI builds a valid 262,144-byte GBC-only ROM, SHA-256
`968d1b33cf271ee04e95dcc9354abe12608c56ffcd0820ca08366f8472185aaa`.
Native runtime recording `game/build/identity-inbounds-delivery`, session
`641c636d2c8641e79630a7ed88ab4273`, stopped **passed** at frame 2042 in PyBoy 2.7.0.

| Native frames | Evidence |
| --- | --- |
| 384–512 | [MATUS → MAGIC → MATUS](identity-inbounds/pass-control.gif), marker/HUD/camera follow both catches |
| 666–936 | [Made basket and CPU inbound](identity-inbounds/cpu-inbound.gif), retrieve/carry/throw without a cut |
| 1288–1600 | [Home inbound](identity-inbounds/home-inbound.gif), A throws from outside baseline and MAGIC receives control |
| 1913–2041 | [CHOMPA → MEAT and shot](identity-inbounds/meat-shot.gif), green beanie, `00`, receiver charge/release |

Source-row palette placement was checked in native OAM. CHOMPA's 14 green pixels
occupy scanlines 63–64 within the head row, with ordinary kit colors on the legs.
Earlier failed contrast/palette-row trials are recoverably archived; their hashes
and paths remain in [machine-readable evidence](identity-inbounds-evidence.json).

The actual exported loader uses **188/192 tiles per reserved sprite-bank span**:
net 4, crew 176, ball 2, jersey marker 6. UI tiles 192–255 stay separate. Gather and
landing share authored cells to fit the identity markers. Sampled OAM peaks at
9/10/7 objects per scanline at frames 512/996/1913, with no sampled over-limit rows.

The official Binjgb browser export was independently packed from the same source,
SHA-256 `f757c724d48d58138c35b78ead6136b832d81a186e49dcab8b40631ee5019027`.
Keyboard selection, a pass to MAGIC, recipient movement, and native pause were
checked. The owned preview remains open, paused on MAGIC 32; [browser evidence](identity-inbounds/browser-paused.png).
Press P to resume or X/B while paused to choose another lineup.

These checks establish this prototype's behavior and sampled rendering. Physical
Chromatic performance and the user's visual/feel acceptance remain unverified.
No completed quarters, final uniforms, records, audio, Dunk Cam, or fire is claimed.

# Animated finishing and court readability

## Changes

Dunk Cam now uses articulated 24×56 cels: deeper anticipation, a cock-back swing, scissoring legs, extension, a swinging rim hang, landing and recovery. BAMBI and CHOMPA gather with two hands; the other four use a one-hand tomahawk motion. Confirmed heights/builds, faces, jerseys and numeric ratings remain intact. Movement, eligibility, scoring and defensive probabilities are unchanged.

Made finishes add speed streaks, a small impact shake, expanding rays, rim rattle, net stretch/sway, crowd-arm movement and landing dust. Failed finishes omit made-basket impact effects. The live ball uses explicit per-cel hand anchors until release; afterward it follows the simulation. Native sprite OAM is published before the same VBlank as the new camera tilemap, correcting the first trial's visible hand/ball timing mismatch.

The court palette changes from warm tan/brown to cool slate/teal: `A8C7C4`, `365663`, `243C50`, `102033`. This separates the surface from warm skin and the orange ball while keeping pale lines and white/red kits readable. Court geometry, collision and camera framing are unchanged. Hardwood can work with a quieter palette; this slate direction is the current review candidate.

## Verification

- `sh scripts/test.sh` passes with sanitizers and warnings as errors: 3,072 dunk cases plus existing selection, shooting, contact, passing and inbound checks. No simulation or supplied-rating changes were needed.
- Official native ROM `a78b168406d11c881bbabcbc639d7c37e525f5ec57a4708d71bfaa8a537af3c6` is a valid 2 MiB GBC-only cartridge. The build succeeds with the existing CLI deprecation/SDCC optimizer warnings.
- Genuine PyBoy 2.7.0 input-driven captures exercise every profile: MAGIC, MATUS, MEAT and CHOMPA finish at the right basket; CPU BAMBI finishes at the left; CHRUX is blocked by BAMBI with no basket or made-impact celebration. Court/inbound or loose-ball returns remain visible. SELECT pause preserves every simulation byte except previous input, including RNG; resume completes the sequence.
- The native cock-back regression was repeated after synchronizing OAM/tilemap publication. The ball now stays with the fast-moving hand. The initial failed trial is retained in a reversible recording archive; its identity is in the evidence JSON.
- Official source export and sprite-budget check pass at **190/192** tiles per reserved bank span. All 336 cinematic frames stay at or below **88** unique tiles. Standing court/portrait art is preserved.
- Official browser export `5a5e0fefe35005ec260f2f61d431cf78fe1a98c1a8c695bd5518de2126afeb61` was played in the existing built-in-browser tab. MAGIC's Enter + X dunk, pause/resume, impact/hang and return with score 02:00 were visually checked. The preview is left natively paused; press P to play.

## Review captures

- [MAGIC dunk and return](finishing-v2/magic.gif)
- [BAMBI two-hand finish at the left basket](finishing-v2/bambi.gif)
- [CHRUX blocked finish](finishing-v2/chrux-block.gif)
- [Slate court in browser](finishing-v2/browser-court-action.png)
- [Browser dunk](finishing-v2/browser-dunk.png), [impact](finishing-v2/browser-impact.png), [return](finishing-v2/browser-return.png)
- [Build/runtime/branch/frame receipts](finishing-v2-evidence.json)

GIFs come from genuine retained native frames; repeated images are coalesced by the exporter. Source studies in `art/dunks-v2/` omit the live ball and are not gameplay evidence. Your prior browser practice state was saved before replacing the preview. The old idle preview's close acknowledgment failed; its owned process was shut down and reopened without deleting saves or journals. Physical Chromatic testing and user visual acceptance remain open. This pass adds no match clocks, fire streaks, final uniforms, audio or persistent records.

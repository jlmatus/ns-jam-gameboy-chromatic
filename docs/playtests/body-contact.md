# Physical Profiles and Contact Playtest

Tested October 28 Managua / October 29 UTC, 2026. This pass addresses the user's
confirmed heights/builds, difficult manual steals, body strength, and directional
movement. It is a new gameplay candidate; final feel acceptance remains with the
user. Four-quarter games and the larger arcade presentation remain future work.

## Resulting Behavior

- Confirmed heights: MATUS 160 cm, MAGIC 190, BAMBI 173, TORRES 170, CHOMPA 185,
  MEAT 173. Builds are stocky, lean, heavy/stocky, lean, broad, lean. No weights
  were supplied. Native standing silhouettes span 36–42 pixels, anchored at the
  feet. Build affects standing torso width and contact radius/mass; action poses
  share some torso pixels to fit the cartridge tile budget.
- A held defensive A reaches at a rating-based cooldown. Separate court/depth
  reach checks stay usable after collision separation. Getting in front improves
  success; behind-the-carrier reaches are weaker. Human reaches can contest after
  eight possession updates, rather than waiting through the CPU's whole protected
  possession. CPU catch protection remains intact.
- Defensive START+A shoves. Power/build determine resistance; a successful shove
  can create a loose ball, not automatic possession. Targets have bounded recovery
  and cannot instantly collect the ball while stunned. SHOVE remains visible
  through the immediate pickup.
- Offensive turbo contact can run over weaker defenders. Strong planted players
  can stop an outmatched drive. Contact cooldowns prevent repeated knockdowns.
  Ordinary overlap moves a body at most one pixel per pair/update.
- CPU diagonal movement follows the human rule. Retreating defenders move at
  three quarters speed while facing the carrier. Height contributes to release,
  block reach, and rebound timing. Supplied ratings and generous Warm-up human
  shooting remain unchanged.

## Verification

`sh scripts/test.sh` passes with AddressSanitizer, UndefinedBehaviorSanitizer,
and warnings as errors. A new 108,000-update mixed-input soak covers every human
and CPU level: 142 reaches, 107 shoves, 66 runovers, and 505 shots, with bounded
positions/recovery and valid ownership. In 512 matched scenarios, MATUS wins
378 front reaches versus 194 from behind. Other checks cover early catch grace,
held-input cooldowns, strength reversals, loose-ball recovery, pause, CPU retreat,
all 1,080 lineup/level combinations, rating effects, and existing match actions.

The official CLI built a valid 262,144-byte GBC-only cartridge:
`2cba824d4e690a6f117cb2bb152c58c564dd00635858fe7901419270bbd63692`.
Contact code lives in a separate banked `jam_contact.c`; the first larger single
simulation unit exceeded the 16 KiB bank limit. The final build has no compiler
errors; retained warnings are Node DEP0190 and SDCC optimizer warning 110.

The exact cartridge was played in PyBoy 2.7.0 with retained inputs and frames:

| Action | Native frames | Observation |
| --- | --- | --- |
| [BAMBI power drive](body-contact/power-drive.gif) | 432–532 | Defender displaced before midcourt; human keeps possession |
| [Shove and recovery](body-contact/shove-recovery.gif) | 664–712 | Loose ball, target stun, human pickup, recovery; SHOVE feedback persists |
| [MATUS front steal](body-contact/matus-front-steal.gif) | 1328–1408 | First valid reach wins possession; one attempt and one steal in WRAM |

Recording `game/build/body-contact-delivery`, session
`e50f0d9a7bb240a484dd0f58786a73e5`, stopped passed at frame 1409. Clips hold genuine
sampled native frames; decision pauses are omitted, and no motion is interpolated.
The [combined clip](body-contact/contact-highlights.gif) contains those three cuts.
Earlier unsuccessful tuning trials are reversibly archived, with references in
[machine-readable evidence](body-contact-evidence.json).

After official source export, `node scripts/check-native-sprite-budget.mjs`
passes at **192/192 reserved sprite-bank span tiles**: net 4, crew 184, ball 2,
marker 2. UI tiles 192–255 remain separate. Future art changes must repeat this
loader check; generic PNG tile estimates do not establish loader safety. Sampled
OAM at frames 532/712/1408 peaks at 9/9/7 objects per scanline, with zero sampled
over-limit scanlines. This is not exhaustive hardware or frame-rate validation.

## Browser and Acceptance Limits

The official Binjgb export built from the same authored source. Its independently
packed ROM is `4bfe553a8638e3d8c66559911de62022cb11795a3da38cfd5dc701fab31005f0`.
Keyboard Z advanced the six selection stages, Right moved MATUS, and P paused
the native game. The owned preview remains open, paused, with
`SELECT PLAY B LINEUP` visible; see [browser capture](body-contact/browser-paused.png).
Press P to resume or X/B to choose another lineup. Z is A; Enter+Z is turbo+A.

The supplied [additional gameplay reference](https://www.youtube.com/watch?v=1wMpgi6oWOk)
was viewed in the built-in browser at selected on-court positions around
3:05, 3:10, and 3:47. These inform basket framing and directional silhouettes;
they do not prove the original game's internal mechanics. This pass retains the
existing 400-pixel court projection rather than adding arbitrary browser zoom.
No physical Chromatic play, final uniforms, audio, Dunk Cam, fire, records, or
four-quarter match flow is claimed. Sustained human play still determines feel.

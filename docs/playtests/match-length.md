# Configurable match length

## Behavior

After CPU difficulty, MATCH LENGTH offers 1, 2 or 4 periods, each three minutes. Four is the default. Arrows select, A confirms, and B goes back. The menu shows 3/6/12 minutes of total live clock time; lineup review repeats the choice. Review's B returns to length, then B returns to difficulty.

The selected last period ends the game at Q1, Q2 or Q4. Earlier periods retain the existing break/inbound behavior. Rematch keeps lineup, difficulty and length; returning to setup retains the length for editing. Only 1/2/4 are accepted by simulation initialization. There is no shortened development timer.

## Verification

- `sh scripts/test.sh` passes with sanitizers and warnings as errors: all 3,240 distinct lineup/difficulty/length combinations, default/wrap/backtracking/held-input behavior, exact 180-second boundaries for every configured length, final freeze, same-length rematch, invalid-length rejection, and the existing clock/shooting/contact/turbo checks. See [host output](match-length/host-tests.txt).
- Official native ROM `1a80e8be707bf2053dd7dbc1d906d7812b23655bc5b6689fb7b64880aa8a3b51` builds with valid GBC-only 2 MiB cartridge headers. The build retains matching debug artifacts. Compiler output contains the existing CLI deprecation and SDCC optimizer warnings, with no errors.
- Genuine PyBoy 2.7.0 frames show the settings page, four-period default/review, one-period review and full game ending at Q1 0:00, a one-period rematch, and a two-period game with Q1 break then Q2 final. Default-four launch still starts Q1 at 3:00. These runs use actual controller inputs and ordinary clock timing; no memory writes or fast clocks.
- Official source export plus sprite-budget verification passes at 190/192 tiles. The setting uses the existing text font and adds no graphics allocations. `git diff --check` passes.

## Browser limitation

The official web compiler exits zero and publishes export ROM `fb1d0d35b1ca166a6f2051a4e92ed1aa704e33a34dbd62a112dff22b8abbe5ff`, but the preview replacement reports `Browser recording close acknowledgement is UNKNOWN`. Status retains the previous listener/build with `closureState: unresolved`; its view reports no recording owner, idle phase and no pending save, but that does not resolve the unknown close. A normal UI resume did not settle it. The existing view is left paused, its prior game state is retained, and the uncertain replacement is not replayed or bypassed. Updated browser-menu interaction is therefore unverified. Native behavior is verified independently. No physical Chromatic test was performed.

## Evidence

[Settings](match-length/settings.png), [one-period review](match-length/review-one.png), [four-period review](match-length/review-four.png), [Q1 final](match-length/one-final.png), [Q2 final](match-length/two-final.png), [four-period start](match-length/four-start.png), and [build/input/frame receipts](match-length-evidence.json).

The bounded native recording retains periodic genuine frames every 60 VBlanks plus sampled call endpoints, with input history and branches. Samples show observed states after period boundaries, not exact buzzer frames. Earlier four-quarter native evidence remains in [quarter verification](quarters.md); overtime and persistent settings/records remain deferred.

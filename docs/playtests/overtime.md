# Overtime verification

## Behavior

Ties after the selected 1/2/4 regulation periods show `TIED A OT B MENU`. A/Z starts a full one-minute overtime, shown as `OT 1:00`. Further ties repeat overtime. Scores, lineup and statistics carry over; turbo refills and inbound possession alternates. Pause, Dunk Cam and dead-ball/inbound clock stops remain unchanged. Released buzzer shots resolve before the winner/tie decision. An untied OT ends with WIN/LOSE and the existing replay/menu controls.

The former 99-point cap is raised to 9,999 with 16-bit scores and expanding HUD fields, allowing a 99–99 tie to be broken. Cartridge records remain deferred.

## Validation

- Sanitizer/warnings-as-errors suite passes: tied games at all three lengths, 260 repeated overtimes per length, exact 3,585-VBlank minute, alternating inbounds, frozen breaks and pause/dead-ball clocks, preserved scores/stats/setup, turbo refill, tying/winning/missed buzzer shots, a real 99–99 to 101–99 finish, transition audio events and rematch reset. Existing rules regressions also pass. See [host output](overtime/host-tests.txt).
- Official CLI native build exits zero. ROM SHA-256: `8024c84c166f58316e81179f101f880403b8f78cf999e950f11b1cba2eaecf1c`. Header inspection confirms valid GBC-only 4 MiB MBC5 cartridge. Existing SDCC optimizer and stock SFX inline-helper warnings remain; no errors.
- Exact-ROM PyBoy 2.7.0 play uses ordinary controller inputs and the full hardware clock, with no memory writes or shortened timer. MATUS/MAGIC vs BAMBI/CHRUX, Warm-up, one regulation period: 2–2 regulation tie at frame 13,432; first OT starts at 13,440; paused clock stays at 0:54 through frame 14,348; repeated tie at 18,136; second OT starts at 18,144 with the alternate inbound; MAGIC scores for 4–2; final at 22,992; rematch resets to Q1 3:00 and 0–0 at 23,008.
- Official source export and sprite-budget check pass at 190/192 tiles. No additional art/tile allocation. `git diff --check` passes.
- Official browser export SHA-256: `7364856c80080dde83949570215be250e5528f460328cfda9e52770f9d3f1b56`. Built-in browser verifies lineup selection, one-period setup, match start, movement and SELECT pause. Left open, paused, for human review. Full OT route was verified independently in the native ROM; physical Chromatic remains untested.

## Evidence

[Regulation tie](overtime/regulation-tie.png), [OT start](overtime/ot-start.png), [live OT](overtime/ot-live.png), [pause](overtime/paused.png), [second tie](overtime/repeat-tie.png), [alternating inbound](overtime/ot-two-start.png), [winning basket](overtime/winning-basket.png), [final](overtime/final.png), [rematch](overtime/rematch.png), [browser](overtime/browser-paused.png), and [build/frame receipts](overtime-evidence.json).

The retained native recording is `game/build/overtime`, session `b54775d85337440dbb295cace7f11dea`, branch `branch-0001`, finalized as passed. Its periodic genuine frames are sampled every 60 VBlanks, plus call endpoints. These samples establish the observed states rather than every intervening frame. The public recording retains the complete input journal for replay. Historical evidence was preserved through reversible plugin archival to release recording quota.

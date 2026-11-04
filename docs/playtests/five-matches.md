# Five full matches — 2026-11-04

## How they were played

The exact cartridge (CLI ROM `0fc2c0d8…4cb4`) was played in PyBoy by [`five-matches/play_match.py`](five-matches/play_match.py). The script presses only the real buttons:

- **Offense:** drive and dunk with START + B, timed jump shots, pump fakes, passes and pass requests.
- **Defense:** stay between the carrier and our rim, swipe with A, shove with START + A, and jump to block with B.
- **Breaks:** press A at each quarter break.

It reads the game state from RAM the way a player reads the screen. To keep it human, it reacts to what it saw 8–10 frames earlier. It also releases shots anywhere from 16 to 38 frames, so it doesn't always hit the timing window (18–36).

Every match is four three-minute quarters. Seeds make each match reproducible.

| # | You + CPU teammate | Opponents | Level | Final |
|---|---|---|---|---|
| 1 | MATUS + MAGIC | BAMBI + CHRUX | Pickup | 79–28 |
| 2 | BAMBI + CHOMPA | MEAT + MATUS | Pickup | 72–29 |
| 3 | CHRUX + MEAT | MAGIC + CHOMPA | Winners Stay | 42–9 |
| 4 | CHOMPA + MATUS | CHRUX + BAMBI | Warm-up | 142–8 |
| 5 | MEAT + BAMBI | MATUS + MAGIC | Winners Stay | 65–38 |

## After the CPU ball-security fix

**What changed:**
- **CPU carriers under pressure** (a defender within 28 pixels) pass to a clearly more open teammate. Otherwise they dribble sideways around a defender standing between them and the rim. Long possessions without pressure also move the ball. This lives in `jam_carrier.c`, because the `jam_sim.c` code bank is nearly full.
- **Your side's reaches** use `14 + 4 × steal` (was `28 + 5 × steal`), with a front bonus of 14 (was 20). Opponents keep their old odds.
- **Your swipes** wait 30 updates after any catch (was 8) and repeat every `40 − 2 × steal` updates (was `30 − 2 × steal`).
- **New rules tests:**
  - 64 of 64 pressured carriers pass to an open teammate.
  - 64 of 64 dribble around a covered lane.
  - An unpressured carrier still attacks the rim.
  - Front reaches still beat reaches from behind (237 vs 78 of 512).

**Same five matches, same seeds,** on ROM `7b8bbba5…3b7e`. The `before/` folder keeps the earlier summaries.

| # | Level | Final before → after | Steals (both teams) | Opponent field goals made/attempted | Median opponent possession | Dunk Cam share of frames |
|---|---|---|---|---|---|---|
| 1 | Pickup | 79–28 → 54–30 | 76 → 70 | 14/25 → 15/34 | 2.7 s → 6.0 s | 15% → 19% |
| 2 | Pickup | 72–29 → **63–66** | 75 → 46 | 14/42 → 33/59 | 3.5 s → 7.0 s | 19% → 24% |
| 3 | Winners Stay | 42–9 → 45–39 | 120 → 61 | 4/17 → 18/34 | 1.4 s → 5.9 s | 23% → 21% |
| 4 | Warm-up | 142–8 → 82–16 | 77 → 38 | 4/8 → 8/12 | 3.1 s → 6.1 s | 12% → 11% |
| 5 | Winners Stay | 65–38 → 57–33 | 96 → 54 | 14/31 → 14/55 | 3.1 s → 5.4 s | 18% → 16% |

**The CPU now keeps the ball and competes.** Opponent possessions roughly doubled in length, steals fell by up to half, and the CPU won match 2.

**Still open:**
- **MATUS as the human player still got 47 steals** in match 1 (steal 8, fast repeat). Some of those may be pass interceptions now that the CPU passes more; the steal counter doesn't separate the two.
- **Dunk Cam's share went up in some matches,** because the CPU now reaches the rim more often. That makes item 2 below more pressing.
- **BAMBI and CHRUX still take almost no jump shots;** with low three-point ratings, they drive.

## Screen shake fix

**What you saw:** the screen sometimes shook, as if a player were stuck.

**Cause:** per-frame traces of the camera scroll found 9–10 shake episodes per quarter. Two things caused them:

1. **The camera aimed at the ball.** A held ball sits on the side the carrier faces, about 11 pixels left or right of the body. A CPU player's facing flipped between sideways and up/down on diagonal runs, so the ball, and the camera following it, jumped back and forth.
2. **The first ball-security version sent a carrier up and down every few frames.** That happened when the defender was level with him, or just at the edge of pressure range.

**Fixes:**
- The camera now follows a held ball by its carrier's body, in `basketball.c`.
- CPU players use the same facing rule as you: any sideways step faces sideways.
- A pressured carrier picks one side and keeps it while edging toward the rim. Just outside reach, he holds his new lane instead of cutting back.

**Result:** 0–3 shake episodes per quarter in the same runs, using `jitter.py` on `JAM_TRACE` traces. The few left come from the scripted player's own left-right inputs while it waits for a pass.

**New tests** check that a level defender causes no up-and-down reversals and that a carrier turns at most twice.

**Test-only change:** the contact soak now checks every player against the legal apron (8–312 pixels). An inbounder may legitimately fetch a ball that went out past 306 pixels; the new movement paths happened to reach that case.

**ROM** `23da6052…efc9`. Replaying all five matches gave finals of 71–42, 51–44, 50–40, 90–22 and 53–34. Median CPU possessions were 4.2–7.6 s (originally 1.4–3.5 s). The `gN/` folders now hold this run.

The rest of this page is the original analysis of the first run.

Each `five-matches/gN/` folder (now holding the after-fix run) contains:
- the box scores (`summary.json`)
- every event (`events.json`)
- the possessions (`possessions.json`)
- position samples every half second (`samples.json`)
- screenshots (`shots/`), including all results pages

Summaries come from `analyze.py` and `possession_ends.py`.

**What works:** all five matches ran from tipoff through four quarters, the breaks and the final results pages. Nothing stalled: there were no stuck loose balls, inbounds or held balls.

## What could be improved, most important first

### 1. The CPU can't keep the ball, so matches aren't contests

**Most opponent possessions end in a steal:**

| Match | Opponent possessions ending in a steal |
|---|---|
| 1 | 62 of 89 |
| 2 | 46 of 95 |
| 3 | 88 of 107 |
| 4 | 73 of 95 |
| 5 | 61 of 106 |

- **Opponent possessions are short:** the median is 1.4–3.5 s, versus about 6 s for ours.
- **Opponents rarely shoot jump shots:** 0, 17, 10, 0 and 29 per match.
- **Our CPU teammate steals too,** not just the scripted player: MATUS 50 (match 4), MEAT 51 (match 3).
- **Difficulty barely changes this.** Winners Stay still ended 42–9 and 65–38.

**Cause, from the code:**
- The CPU carrier walks toward a spot near the rim and only shoots or passes in narrow conditions.
- It never protects the ball or moves away from a defender.
- Your team gets 60–90 updates of steal protection after a catch; CPU carriers get none.

**Improvements:**
- CPU carriers pass out of pressure and pull back from a defender.
- They shoot from their real range instead of only near the rim.
- Give them the same post-catch protection your team gets.
- Cap steals when the defender is beside or behind the carrier.

### 2. Dunk Cam takes too much of the game

- **31–61 dunk attempts per match,** each about 4 seconds of close-up.
- **12–23% of all frames are Dunk Cam:** 15%, 19%, 23%, 12% and 18%.
- **Many attempts don't score:** only 17 of 61 in match 3, and 25 of 48 in match 5.

The six new dunks will still get repetitive at this rate.

**Improvements (any one helps):**
- Show the close-up only for some dunks: turbo dunks, the first of each quarter, or ones that change the lead. The rest finish on the court.
- Let A skip the close-up.
- Make the turbo cost of a dunk higher, so dunks are rarer and bigger.

### 3. Blocking by big players is too strong

- **MAGIC blocked 80 shots in match 3** (Winners Stay). His chance per attempt is `10 + 7 × 10 = 80%`, plus 8 on Winners Stay.
- **Our side shot very poorly there:** CHRUX made 8 of 59 and MEAT 12 of 69.
- **Even low-rated blockers block a lot:** BAMBI (block 4) blocked 23 in match 1.
- **Blocks per match:** 39, 50, 109, 24 and 65.

**Improvements:**
- Lower the per-attempt chance (for example `10 + 5 × block`).
- Allow at most one block try per possession window.
- Make a blocked shot sometimes stay with the shooting team.

### 4. Contact happens constantly and players pile up

- **Players are knocked down 31–85 times per match** (runovers), plus 4–21 shoves.
- **Screenshots often show three or four bodies stacked on one spot**, which makes it hard to read who has the ball ([match 1 samples](five-matches/g1/shots)).

**Improvements:**
- Fewer runovers: require turbo plus a power advantage, with a longer cooldown.
- Off-ball spacing: the CPU teammate and the second defender keep about 24 pixels away from the ball.

### 5. Off-ball players don't create anything

- **Your CPU teammate always trails 32 pixels behind the ball carrier,** above or below the rim line. It never moves to an open spot or toward the rim.
- **The second defender mirrors the carrier.**

That's why passes rarely lead to anything. Making the teammate move to an open spot, or toward the rim when its defender is beaten, is also what an alley-oop would need later.

### 6. Shooting balance

- **Elite shooters are very reliable from the arc:**
  - CPU-teammate MATUS made 19 of 27 threes (match 4).
  - Opponent MATUS made 10 of 18 (match 5).
- **Deep shots barely lose accuracy.** A timed MATUS three is about 82% at 100 pixels and still about 66% at 150 pixels. The drop-off should be much steeper past about 130 pixels so a deep heave is a gamble.

### 7. Smaller notes

- **Traveling is easy to trigger by accident** (4–7 per match here). After a pump fake, any D-pad press is a travel. The scripted player shows how easy it is; a gentler rule would allow a one-step pivot.
- **The results screens read clearly.** They show both winners, every player's stats, "A REMATCH" and "B CHANGE LINEUP".

## Limits

- The scripted player has perfect knowledge of positions and never tires. That may overstate how easily a person steals the ball, but the CPU teammate's steal totals show the same weakness.
- None of the matches went to overtime.
- Only Pickup and Winners Stay were tried more than once.
- No sound was checked, and nothing was tested in the browser or on hardware.

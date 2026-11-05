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

## Interceptions, CPU shot choice and the buzzer — follow-up

**Changes:**
- **Interceptions follow the passer's rating:** chance `30 + 4 × steal − 5 × passing` (at least 5%; was `45 + 4 × steal − 3 × passing`), and reach shrinks by 2 per passing point.
  - A MAGIC pass (passing 5) through a MEAT lane (steal 8) is now picked off 36% of the time, versus 62% before.
  - A MATUS pass (passing 10) through the same lane: 12%, versus 47% before.
- **CPU passes only go through a clear lane:** no upright opponent within 16 pixels of the line.
- **Mid-range** two-pointers (48 pixels out to the arc) use `52 + INSIDE + three-point`. Close shots keep `60 + 3 × INSIDE`.
- **CPU shot choice** (`jam_carrier.c`) works from the chance at the spot, a close defender's contest included, rather than from position:
  - it shoots a two at 55% or better, or a three at 40% or better;
  - late in a possession it takes any look of 35% or better;
  - in dunk range with a clear lane, it rises for a dunk;
  - through a defender it attacks the rim at `(INSIDE − 5) × 15%` (MAGIC 60%, CHRUX and MEAT 45%, BAMBI and CHOMPA 30%), and otherwise pulls up or passes.
- **Dribble-around:**
  - when the defender is level, the carrier picks the side toward the middle of the court and keeps it for the possession;
  - its target stays 12 pixels inside the sidelines.
- **Buzzer fix:** pressing B on the buzzer used to carry into the break screen, where B means MENU, and quit the match in progress. The script did exactly that in match 5. Now the break ignores A/B for half a second, and ignores any button still held from play until it's released. `buzzer-b/` replays the case with B held through every buzzer: all four quarters and the final are reached.
- **Tests:**
  - **New:**
    - lane interceptions by passer rating (MAGIC 91 vs MATUS 31 of 256);
    - CPU shot choice: open-lane dunks 64/64; in a guarded lane, dunks, pull-ups and passes, never a forced dunk; mid-range jumpers 64/64;
    - the mid-range formula.
  - **Updated:** older fixtures that assumed instant CPU windups or the old odds. Soak tests now check every player against the legal apron (8–312 pixels).

**Same seeds, previous commit → this build** (ROM `38d48ed4…bf3f`):

| # | Level | Final | CPU interceptions | CPU jump shots (blocked) | CPU dunks | Dunk Cam share | CPU field goals made/attempted | Out of bounds |
|---|---|---|---|---|---|---|---|---|
| 1 | Pickup | 71–42 → 50–20 | 23 → 1 | 0 → 48 (33) | 38 → 11 | 20% → 8% | 21/38 → 10/59 | 1 → 0 |
| 2 | Pickup | 51–44 → 41–35 | 20 → 12 | 16 → 71 (48) | 36 → 9 | 21% → 13% | 21/52 → 15/80 | 1 → 1 |
| 3 | Winners Stay | 50–40 → 51–26 | 21 → 4 | 6 → 45 (25) | 33 → 5 | 26% → 16% | 20/39 → 12/50 | 1 → 2 |
| 4 | Warm-up | 90–22 → 78–14 | 4 → 1 | 0 → 24 (16) | 18 → 9 | 14% → 11% | 11/18 → 7/33 | 7 → 6 |
| 5 | Winners Stay | 53–34 → 67–29 | 22 → 11 | 62 → 53 (29) | 5 → 9 | 14% → 19% | 14/67 → 13/62 | 0 → 0 |

**Results:**
- **Interceptions collapsed:** 1–12 per match, from 4–23.
- **Every CPU player now takes jump shots,** dunks are rarer, and Dunk Cam takes 8–19% of play, down from 14–26%.
- **CPU scoring fell** (14–35 points), because **52–69% of CPU jump shots are blocked**. That's the over-strong blocking from item 3 below, and the next thing to fix.

## Blocking tuned — follow-up

**Changes:** jump-shot blocks now need a jumping defender who is close and not behind the shooter.
- **Odds:** `5 + 5 × block` (was `10 + 7 × block`): MAGIC 55% (was 80%), BAMBI 25% (was 38%), MATUS 15% (was 24%).
- **Reach:** `48 + 3 × block` quarter pixels plus height (was `80 + 4 × block`), so about 12–21 pixels instead of 22–32.
- **Unchanged:** dunk blocks keep their own contest rules.
- **Tests:**
  - **New:** MAGIC blocks 141 of 256 from in front; 0 from behind; 0 out of reach.
  - **Still passing:** the earlier block-rating comparison (MAGIC 141 vs MATUS 38).

**Same seeds, three builds:** commit `4fd5d5d` → interceptions and shot choice → this build (ROM `2b52425b…0968`). The block columns compare the middle build with this one.

| # | Level | Final | CPU jumpers blocked | Your jumpers blocked | All blocks | CPU dunks | Dunk Cam share | CPU field goals made/attempted |
|---|---|---|---|---|---|---|---|---|
| 1 | Pickup | 71–42 → 50–20 → **52–48** | 33/48 → 8/33 | 11/36 → 8/47 | 56 → 21 | 11 → 7 | 8% → 6% | 10/59 → 24/40 |
| 2 | Pickup | 51–44 → 41–35 → **41–62** | 48/71 → 17/59 | 12/20 → 3/32 | 68 → 30 | 9 → 14 | 13% → 12% | 15/80 → 28/73 |
| 3 | Winners Stay | 50–40 → 51–26 → **62–28** | 25/45 → 12/32 | 35/45 → 20/41 | 73 → 49 | 5 → 1 | 16% → 17% | 12/50 → 12/33 |
| 4 | Warm-up | 90–22 → 78–14 → **107–14** | 16/24 → 2/9 | 9/42 → 2/50 | 33 → 7 | 9 → 3 | 11% → 9% | 7/33 → 7/12 |
| 5 | Winners Stay | 53–34 → 67–29 → **70–51** | 29/53 → 9/40 | 13/26 → 7/37 | 59 → 25 | 9 → 6 | 19% → 14% | 13/62 → 20/46 |

**Results:**
- **Pickup and Winners Stay matches are contests again;** the CPU won match 2.
- **Fewer blocks:** total blocks per match roughly halved, and only 6–29% of CPU jumpers are blocked (was 52–69%).
- **Warm-up stays one-sided,** as an easy level should.
- **Match 3 still has many blocks** (49): MAGIC and CHOMPA are the CPU's two best blockers, and dunk blocks are unchanged.
- **Dunk Cam takes 6–17% of play.**

## Body contact: held up, never pushed out — follow-up

**What you noticed:** BAMBI pushes you toward the lines.

**Confirmed in a rules experiment:** you (MATUS) drive right along the upper half of the court with one CPU defender in front.
- With BAMBI or CHOMPA defending, you ended up out of bounds in 32 of 32 runs, pushed up to 20 pixels toward the top sideline.
- With CHRUX or MEAT defending, you were pushed 0 times.

**Cause:**
1. When two bodies overlap, the weaker one moves. Strength is power plus build, and BAMBI is the strongest player.
2. The move was almost always sideways, even when you ran straight into the defender.
3. CPU defenders sit slightly below the carrier, so the push usually went up.
4. Nothing stopped a push or knockback at the line.

**Changes** (`jam_contact.c`):
- **Separation follows the shallower overlap,** up to 2 pixels per update. Running into a stronger defender now stops you ("BODY HELD GROUND") instead of sliding you.
- **Separation never moves a player from the court across a line.** If the weaker body has no room, the stronger one gives way.
- **Shove and runover knockbacks** (`jam_knockback_step`) stop at the line.
- **Unchanged:** BAMBI still wins contact, through his turbo runover and his shove.

**New rules test:** driving into BAMBI or CHOMPA, in mid-court and beside the sideline:
- you stay on the court;
- you're held within 4 pixels of your lane;
- a knockback toward the line stops at it.

**A/B on the same seeds:** the previous commit (`ecfd61f`) against this build (ROM `7458ef0a…a8e7`), three seeds per lineup. Summaries are in `five-matches/contact-ab/`.

| # | Level | Lineup | Previous build | This build | Out-of-bounds calls (3 matches) |
|---|---|---|---|---|---|
| 1 | Pickup | MATUS+MAGIC vs BAMBI+CHRUX | 48–50, 77–38, 74–36 | 84–28, 69–26, 75–22 | 2 → 0 |
| 2 | Pickup | BAMBI+CHOMPA vs MEAT+MATUS | 51–62, 40–64, 62–59 | 51–40, 68–29, 59–39 | 2 → 5 |
| 3 | Winners Stay | CHRUX+MEAT vs MAGIC+CHOMPA | 57–36, 68–15, 55–26 | 61–39, 68–48, 67–37 | 5 → 1 |
| 4 | Warm-up | CHOMPA+MATUS vs CHRUX+BAMBI | 80–14, 93–14, 110–20 | 79–26, 80–22, 88–18 | 40 → 2 |
| 5 | Winners Stay | MEAT+BAMBI vs MATUS+MAGIC | 52–47, 51–53, 65–50 | 72–64, 58–44, 63–78 | 2 → 0 |

**Results:**
- **Out-of-bounds calls fell from 51 to 8** across the 15 matches.
- **Against a CPU BAMBI** (lineup 1), the CPU now scores less, because pushing you out had been part of its "defense".
- **Lineups 2 and 3 moved in opposite directions.**
- **Winners Stay is still close,** and the CPU won one.

**Shake check:** the `gN/` folders and `buzzer-b/` hold this build's single-seed run. The buzzer case still completes all four quarters.

The rest of this page is the original analysis of the first run.

Each `five-matches/gN/` folder (now holding the after-fix run) contains:
- the box scores (`summary.json`)
- every event (`events.json`)
- the possessions (`possessions.json`)
- position samples every half second (`samples.json`)
- screenshots (`shots/`), including all results pages

Summaries come from `analyze.py` and `possession_ends.py`.

**What works:** all five matches ran from tipoff through four quarters, the breaks and the final results pages. Nothing stalled: there were no stuck loose balls, inbounds or held balls.

## Dunk Cam only for big dunks, and deep shots a gamble — follow-up

This addresses items 2 and 6 below.

**Dunk Cam** (`jam_dunk.c`, `basketball.c`):
- **The close-up plays only for dunks that matter:**
  - a team's first dunk of each period;
  - a dunk that would tie the game or take the lead;
  - any dunk in the last 30 seconds of a period.
- **Other dunks finish on the court.** It's the same jump, timing, blocks and result, without the close-up ([on-court dunk frames](five-matches/cam-ab/court-dunk.png)).
- **A skips the close-up.** The dunk finishes on the court, and its result doesn't change.

**Deep shots** (`jam_sim.c`): beyond 130 pixels from the rim, a jump shot loses another 1.5 points per pixel, on top of the existing drop past 100.

| Distance from rim | 100 px | 130 px | 150 px |
|---|---|---|---|
| MATUS, timed (Warm-up) | 82% | 72% | 36% (was 66%) |
| MAGIC, timed | 44% | 34% | 5% |

**New rules test:** MATUS at 100/130/150 pixels; MAGIC at 150; the close-up choice (first of the period, tying or go-ahead, the last 30 seconds, a comfortable lead); and A skipping the close-up mid-dunk.

**A/B on the same seeds:** the previous commit (`9c3ddae`, ROM `7458ef0a…`) against this build (ROM `2f3ad604…`), three seeds per lineup. The script now also counts frames shown in the close-up and every jump shot's distance. Summaries are in `five-matches/cam-ab/`, and `cam_ab.py` tabulates them.

| # | Close-up share of all frames | Close-ups / dunks | Shots from 130+ px made | Previous scores | This build's scores |
|---|---|---|---|---|---|
| 1 | 7.7% → 3.9% | 55/55 → 28/61 | 6/10 → 7/16 | 84–28, 69–26, 75–22 | 90–20, 75–28, 68–18 |
| 2 | 10.7% → 5.4% | 79/79 → 39/82 | 2/7 → 0/6 | 51–40, 68–29, 59–39 | 64–46, 82–17, 48–26 |
| 3 | 15.0% → 6.2% | 120/120 → 47/113 | 5/16 → 1/17 | 61–39, 68–48, 67–37 | 39–43, 58–22, 43–45 |
| 4 | 8.4% → 3.5% | 61/61 → 27/54 | 2/6 → 0/4 | 79–26, 80–22, 88–18 | 86–28, 83–14, 82–24 |
| 5 | 14.1% → 7.5% | 113/113 → 58/115 | 27/61 → 13/57 | 72–64, 58–44, 63–78 | 40–42, 51–36, 47–55 |

**Results:**
- **Dunk Cam fell from 11.3% to 5.3% of all frames.** 199 of 425 dunks got the close-up (47%). An on-court dunk takes about 90 frames; a close-up about 230.
- **Matches are about 5% shorter** in real time (866,882 vs 914,836 frames for 15 matches).
- **Shots from 130+ pixels went from 42 of 100 to 21 of 100.** Opponent MATUS in lineup 5 is most of them: 37 of 68 before, 18 of 61 now.
- **Winners Stay got closer:** the CPU won two of three in lineup 3 and two of three in lineup 5. Part of this is the script: it doesn't adapt to the new falloff and keeps taking deep shots, and its controlled player changes with each catch. A person would stop heaving.
- **The script pressed A during 8 close-ups** while doing something else, which skipped them.

**Not refreshed:** the `gN/` folders and `buzzer-b/` still hold the previous build's run.

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

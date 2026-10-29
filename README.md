# NICASOURCE JAM

NICASOURCE JAM is a **2-on-2 arcade basketball game for ModRetro Chromatic**, targeting Game Boy Color. The full game will star six real players from the Nicasource pickup crew in Managua, wearing the team's real uniforms.

**Status: development prototype with scouting cards, player selection, and ratings.** The editable [native project](game/project.gbsproj) builds into a real GBC-only cartridge. Choose any of the six crew members, a CPU teammate, two distinct opponents, and one of three CPU levels. Review the lineup before starting practice on the scrolling court. Movement, dribbling/possession, passing, interceptions, shooting, two/three-point scoring, rebounds, steals, blocking, and possession resets are implemented. The game remains far from final presentation.

The current build uses the crew’s confirmed heights and builds for **36–42-pixel athletic silhouettes on 16×48 sprite canvases**. Bodies have smaller heads, bent knees, directional turns, and distinct shot poses. The 320-pixel logical court is authored across 400 display pixels for a 25% closer horizontal view. Shots gather, rise, release, follow through, and land; the ball rises with the hands. Baskets face inward and use a visible rim/net sequence. White/red kits remain temporary. See [motion and framing verification](docs/playtests/athletic-motion.md), [selection/ratings verification](docs/playtests/selection-ratings.md), and [scouting-card layout verification](docs/playtests/scouting-card-layout.md).

The prototype is continuous practice with native portrait menus, unique lineup selection, rating effects, and three difficulty choices. Four-quarter matches, final uniforms, hot-hand effects, audio, and cartridge records remain later milestones. START provides a simple sprint; turbo + B near the attacking rim triggers an actual dunk with an exaggerated jump, a close-up Dunk Cam, and a return to the court. The final turbo meter remains planned. See [arcade finishing design](docs/design/dunks.md) and [verification](docs/playtests/dunks.md).

The identity/possession pass adds distinct heads and skin colors, a readable controlled-player jersey marker, catch-based control switching, and a visible baseline inbound. See [verification](docs/playtests/identity-inbounds.md).

The contact pass makes front positioning improve steals, lets strong players shove and turbo through weaker bodies, and adds bounded knockback/recovery. CPU diagonal movement now follows the human rule; retreating defenders backpedal more slowly while facing the ball. Supplied ratings remain unchanged; the subsequent shooting pass corrects human roll bias and separates three-point specialists from average shooters. See [contact verification](docs/playtests/body-contact.md).

The latest card/shooting pass revises MATUS’s face proportions and uses the approved dark navy courtside card background with teal and gold accents. Shooting now uses unbiased rolls, a stronger rating distinction, and a deep-range penalty. A timed open three near the arc is 44% for MAGIC versus 82% for MATUS; difficulty still changes opponents. See [card and shooting verification](docs/playtests/cards-shooting-v2.md).

## Play the Prototype

Ask Codex to select `game/project.gbsproj` and run the ModRetro plugin's `web_preview {}` in the built-in browser. The preview URL is local to its running plugin session. Use the onscreen controls or:

| Keyboard | Handheld | Prototype action |
| --- | --- | --- |
| Arrow keys | D-pad | Move your selected player (yellow marker) |
| Z | A | Pass/request a pass; hold to attempt steals on defense |
| Enter + Z | START + A | Shove on defense; power/build determine whether the target yields |
| X | B | Hold then release to shoot; block on defense. Enter + X near the attacking rim attempts a dunk. |
| Enter | START | Hold to sprint |
| P | SELECT | Pause/resume |

The scouting cards use 64×64 approved portraits, double-size names, bold text, and rating bars. In selection, use arrows to cycle players, **Z/A** to confirm, **X/B** to go back, and **P/SELECT** to cycle the card and two full-rating pages. B closes a rating page first. Used players are skipped. At lineup review, **Z/A** or **Enter/START** begins a fresh practice. During practice, **P/SELECT** pauses; **X/B while paused** returns to selection. You start with your selected player. Control switches to your teammate when they catch your pass or gain possession; the jersey marker, HUD name/number, and camera follow the new controlled player. A held pass button cannot immediately pass the catch away again. On defense, you keep control of the last controlled teammate until your team wins the ball.

Attack the right basket; CPU opponents attack left. Follow the ball; the HUD directs you back if your player leaves the view. After a made basket, the other team retrieves the ball, walks outside its own baseline, and inbounds to a teammate. Press A/Z when your inbounder is ready; CPU inbounders throw automatically. A long idle wait also throws automatically to keep practice moving. The inbound throw is protected, and the passer walks back onto court. A possession timeout and bounded loose-ball recovery keep practice moving. Scores cap at 99 in this prototype. The full game's quarter clock has not been added yet.

For a dunk, approach the basket from inside the court, align within 18 court pixels of its depth lane, hold **Enter/START**, and press **X/B**. Stronger dunk ratings permit longer takeoffs and higher jumps. The ball can be blocked or miss; points count only after it crosses below the rim. The camera returns to an inbound after a make or a loose-ball rebound after a failed finish. **P/SELECT** also pauses Dunk Cam.

On defense, align your feet with the carrier’s lane and get in front before holding A. Turbo + A is a shove; release turbo to use a reach instead. On offense, hold turbo while driving into a defender: BAMBI/CHOMPA have the power to displace lighter players, while an outmatched charge can be stopped. Height affects shot release, block reach, and rebound timing; build affects contact size and resistance.

See [PLAN.md](PLAN.md) for the major feature milestones, first playable scope, validation gates, and open decisions.

## Game Direction

Inspired by [NBA Jam '99 on Game Boy Color](https://www.youtube.com/watch?v=7HKh1YjBr1s), NICASOURCE JAM aims for quick, exaggerated basketball with a side view of the full court, turbo, big dunks, a close-up **Dunk Cam**, and a hot-hand streak that sets the net on fire.

The video is a design reference for the project. Exact camera behavior, animation timing, and technical implementation remain to be specified. Player identities, uniforms, and presentation will be specific to the Nicasource crew.

### Reference Video Review

The additional [Game Boy Color gameplay reference](https://www.youtube.com/watch?v=1wMpgi6oWOk) was opened in the built-in browser. Selected views at 3:05, 3:10, and 3:47 show basket framing, clustered court-depth play, and directional running. These guide readability and motion; they do not establish the original game’s internal physics or probabilities.

Browser playback succeeded on October 28, 2026. Selected segments were visually reviewed; this was not a complete frame-by-frame analysis.

- [Lineup at 1:00](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=60s): two player portraits and side-by-side ratings provide a useful reference for our player-selection screen.
- [Gameplay at 1:25](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=85s) and [2:30](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=150s): the angled side view scrolls across the court; the entire court is not visible at once. Keep player and ball readability central to our camera prototype.
- [Basket action at 5:01](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=301s): upright player sprites and a visible basket support readable jumping and finishing near the rim. Dunk Cam is now implemented and verified separately; hot-hand effects remain future work.

## Planned Game Flow

1. Pick the player you control.
2. Pick a CPU teammate.
3. Pick two opponents.
4. Choose a CPU level: **Warm-up**, **Pickup**, or **Winners Stay**.
5. Play a game of **four quarters**.
6. Show the match result. Cartridge-backed records are a later milestone after the complete match loop is playable.

Controls and quarter length follow the Game Boy Color edition of NBA Jam '99, as specified below. Tie handling and detailed rules remain to be designed. Difficulty adjusts opponent movement, defensive reaction time, shooting range, cooldowns, and success chances; your teammate retains the selected player’s ratings. Saved statistics and win/loss attribution are deliberately deferred until the complete game is playable.

### Confirmed Controls and Timing

Use NBA Jam '99's default Game Boy Color configuration, verified in its [offensive control chart](https://www.manualslib.com/manual/4365804/Nintendo-Nba-Jam-99.html?page=21) and [defensive control chart](https://www.manualslib.com/manual/4365804/Nintendo-Nba-Jam-99.html?page=22).

| Input | Offense | Defense |
| --- | --- | --- |
| D-pad | Move | Move |
| A | Pass or request a teammate's pass | Attempt a steal |
| B | Hold for a jump shot; combine with turbo for a dunk | Jump to block |
| START | Hold for turbo | Hold for turbo |
| SELECT | Pause/resume | Pause/resume |

SELECT's pause behavior is documented on [manual page 17](https://www.manualslib.com/manual/4365804/Nintendo-Nba-Jam-99.html?page=17). Use **four quarters of three minutes each**, matching [manual page 15](https://www.manualslib.com/manual/4365804/Nintendo-Nba-Jam-99.html?page=15). Implement the original input feel through prototype testing.

## Player Roster

Names, positions, and jerseys preserve the initial brief. The user-approved playstyle labels below describe the supplied ratings; they are not extra ratings or abilities. Original signature strings remain preserved in the C roster. See [playstyle analysis](docs/design/playstyles.md).

| Player | Jersey | Position | Playstyle |
| --- | --- | --- | --- |
| MATUS | 13 | GUARD | 3PT PLAYMAKER |
| MAGIC | 32 | BIG | RIM PROTECTOR |
| BAMBI | 23 | WING | POWER FINISHER |
| TORRES | 7 | GUARD | SPEED SLASHER |
| CHOMPA | 74 | BIG | STRETCH BIG |
| MEAT | 00 | GUARD | TWO-WAY SLASHER |

Jersey numbers are display strings in structured data: CHOMPA is `"74"` and MEAT is `"00"`. Preserve MEAT's two zeros when rendering the roster and uniform.

### Confirmed Physical Profiles

| Player | Height | Build |
| --- | --- | --- |
| MATUS | 160 cm | Stocky |
| MAGIC | 190 cm | Lean |
| BAMBI | 173 cm | Heavy/stocky |
| TORRES | 170 cm | Lean |
| CHOMPA | 185 cm | Broad |
| MEAT | 173 cm | Lean |

`art/roster-physics.json` stores these confirmed profiles. No weights were supplied; the simulation uses an arcade mass class, not invented kilograms. Regenerate the C table with `node scripts/create-body-profiles.mjs` and native body art with `node scripts/create-prototype-art.mjs`.

### Ratings

All ratings use a **1–10** scale. The supplied values below are implemented. See [rating effects and difficulty formulas](docs/design/ratings.md) for the current starting balance. “Three-point” corresponds to `three_point` in structured data.

| Player | Speed | Three-point | Passing | Steal | Block | Dunk | Power | Clutch |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| MATUS | 7 | 10 | 10 | 8 | 2 | 3 | 5 | 9 |
| MAGIC | 4 | 5 | 5 | 3 | 10 | 9 | 7 | 7 |
| BAMBI | 4 | 5 | 6 | 4 | 4 | 7 | 10 | 7 |
| TORRES | 10 | 3 | 7 | 7 | 4 | 8 | 4 | 5 |
| CHOMPA | 5 | 8 | 5 | 3 | 7 | 7 | 9 | 6 |
| MEAT | 9 | 2 | 5 | 8 | 6 | 8 | 6 | 4 |

## Development Workflow

We use the [ModRetro Chromatic Plugin](plugin://modretro-chromatic@openai-curated-remote), GB Studio CLI 4.3.2, GBDK 4.5.0, and PyBoy 2.7.0. Basketball uses a project-local `BASKETBALL` engine scene; the stock engine is not ejected. Approved portrait pixels are preserved and packed directly into the native selection menus.

The development loop is:

1. Create and select the intended native `.gbsproj` project, preserving its `.gbsres` resources and authored assets.
2. Use the plugin's scene, event, collision, and asset tools; evaluate targeted extensions if the basketball mechanics require them.
3. Build and inspect the actual ROM with `rom_build` and `rom_inspect`.
4. Use `web_preview` in Codex's built-in browser and input-driven emulator checks to verify gameplay.
5. Verify cartridge saves and physical Chromatic behavior before claiming hardware readiness.

Verified commands from the repository root:

```sh
sh scripts/test.sh          # C rules, 3,072 dunk cases, and CPU/contact soaks with sanitizers
sh scripts/build.sh rom    # game/build/rom/nicasource-jam.gbc
sh scripts/build.sh web    # official export into game/build/manual-web/
sh scripts/build.sh export # generated GBDK source into game/build/source/
```

The scripts default to the setup-owned macOS ARM64 Node 24.15.0 and toolchain under `~/Library/Application Support/modretro-chromatic/`. `JAM_NODE`, `JAM_GB_STUDIO_CLI`, and `GB_STUDIO_SETUP_ROOT` support other installations; tests need a C compiler with AddressSanitizer/UBSan. No npm installation is needed. Missing dependencies should be prepared with the plugin setup skill.

`sh scripts/chromatic.sh` starts the installed plugin's MCP launcher under managed Node 24, with this repository as its workspace. It accepts `JAM_CHROMATIC_PLUGIN` for a different installed package path. This resolves the task's inherited Node 20 mismatch without editing Codex/plugin configuration or reinstalling the plugin. The existing Codex-bound session may still report Node 20; use the managed session for setup/build/preview work. Native builds and emulator boots have been verified. See [playtest evidence](docs/playtests/first-playable.md) for exact ROM identities and limits.

## Planning Priorities & Open Decisions

- **Rules and input feel:** implement the confirmed NBA Jam '99 mapping and three-minute quarters; refine input timing, possession rules, and tie handling through the prototype.
- **Ratings and AI:** all eight rating effects and three CPU levels are implemented; further balancing follows later playtests.
- **Presentation:** use the approved default portraits and confirmed jersey numbers; obtain real team uniform references and define court design, HUD, sound, and signature behavior.
- **Dunks and hot hand:** actual dunks, exaggerated jumps, and Dunk Cam transitions are implemented; continue feel tuning. Define hot-hand activation/reset conditions and effects on play.
- **Hardware feasibility:** assess full-court readability, four-player animation, ball logic, camera effects, palette use, and performance within native Game Boy Color limits.
- **Persistent records (deferred):** decide tracked statistics, per-player win/loss attribution, save timing, storage layout, and reset behavior after the complete game is playable. Live match scoring remains part of the core game.

The ordered implementation roadmap is maintained in [PLAN.md](PLAN.md). Saved-statistics and CPU win/loss attribution decisions remain deferred until the complete match loop is playable.

## Repository & Contributions

- `README.md`: game brief, roster, planned workflow, and open decisions.
- `AGENTS.md`: contributor and agent instructions.
- `PLAN.md`: major feature milestones and acceptance checks.
- `game/`: editable project, native resources, assets, and basketball engine extension.
- `tests/` and `scripts/`: simulation tests, managed build commands, and editable art generation.
- `docs/playtests/`: runtime evidence and known prototype limits.
- `.gitignore`: generated ROMs, build directories, local dependencies, environment files, emulator saves, and OS metadata. Authored art and native project resources remain tracked.
- `art/portraits/`: standalone portrait concepts, editable pixel data, and generation references. [MATUS v1](art/portraits/matus-v1/README.md) remains the approved style reference; [MATUS v2](art/portraits/matus-v2/README.md) revises face proportions for review in the current selection menu.
- [BAMBI v1](art/portraits/bambi-v1/README.md): approved portrait using the same size and palette; integrated into the selection menu.
- [MAGIC v1](art/portraits/magic-v1/README.md): approved portrait with rectangular glasses and a broad smile, using the same size and palette; integrated into the selection menu.
- [TORRES v1](art/portraits/torres-v1/README.md): approved portrait with short textured hair and a full beard, using the same size and palette; integrated into the selection menu.
- [CHOMPA v2](art/portraits/chompa-v2/README.md): approved default portrait, jersey 74, based on the closer photograph with a lime beanie. The color portrait is the default; its three-color variant remains available for integration. [V1](art/portraits/chompa-v1/README.md) is an earlier, unapproved study.
- [MEAT v1](art/portraits/meat-v1/README.md): approved default portrait, jersey 00. Its invented origin remains documented; it is the selected design despite the absence of a supplied photo.

See [Repository Guidelines](AGENTS.md) before contributing. Preserve the supplied roster and ratings, and distinguish implemented prototype behavior from planned features.

# NICASOURCE JAM

NICASOURCE JAM is a planned **2-on-2 arcade basketball game for ModRetro Chromatic**, targeting Game Boy Color. It stars six real players from the Nicasource pickup crew in Managua, wearing the team's real uniforms.

**Status: game planning and portrait exploration.** No native game project, playable build, or development toolchain has been created or configured in this repository. Standalone player-face artwork is being explored using the ModRetro Chromatic pixel-art skill. The gameplay sections below describe the intended game, not implemented features.

See [PLAN.md](PLAN.md) for the major feature milestones, first playable scope, validation gates, and open decisions.

## Game Direction

Inspired by [NBA Jam '99 on Game Boy Color](https://www.youtube.com/watch?v=7HKh1YjBr1s), NICASOURCE JAM aims for quick, exaggerated basketball with a side view of the full court, turbo, big dunks, a close-up **Dunk Cam**, and a hot-hand streak that sets the net on fire.

The video is a design reference for the project. Exact camera behavior, animation timing, and technical implementation remain to be specified. Player identities, uniforms, and presentation will be specific to the Nicasource crew.

### Reference Video Review

Browser playback succeeded on October 28, 2026. Selected segments were visually reviewed; this was not a complete frame-by-frame analysis.

- [Lineup at 1:00](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=60s): two player portraits and side-by-side ratings provide a useful reference for our player-selection screen.
- [Gameplay at 1:25](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=85s) and [2:30](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=150s): the angled side view scrolls across the court; the entire court is not visible at once. Keep player and ball readability central to our camera prototype.
- [Basket action at 5:01](https://www.youtube.com/watch?v=7HKh1YjBr1s&t=301s): upright player sprites and a visible basket support readable jumping and finishing near the rim. Our dedicated Dunk Cam and hot-hand effects still need their own design and validation.

## Planned Game Flow

1. Pick the player you control.
2. Pick a CPU teammate.
3. Pick two opponents.
4. Choose a CPU level: **Warm-up**, **Pickup**, or **Winners Stay**.
5. Play a game of **four quarters**.
6. Update cartridge-backed win-loss and stat records for every player.

Quarter length, tie handling, detailed rules, controls, and how results are credited to teammates and opponents are open decisions. The difficulty names are confirmed; their exact behaviors are not yet defined.

## Player Roster

Names, positions, signatures, and known jersey numbers are part of the initial brief. Signature labels do not yet define special-move mechanics.

| Player | Jersey | Position | Signature |
| --- | --- | --- | --- |
| MATUS | 13 | GUARD | TOP OF THE KEY |
| MAGIC | 32 | BIG | OVER THE TOP |
| BAMBI | 23 | WING | BULLDOZER |
| TORRES | 7 | GUARD | BACK DOOR |
| CHOMPA | Unknown (`null`) | BIG | STRETCH BIG |
| MEAT | Unknown (`null`) | GUARD | BIG ENERGY |

### Ratings

All ratings use a **1–10** scale. These are the supplied starting values; their gameplay formulas remain to be designed. “Three-point” corresponds to `three_point` in structured data.

| Player | Speed | Three-point | Passing | Steal | Block | Dunk | Power | Clutch |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| MATUS | 7 | 10 | 10 | 8 | 2 | 3 | 5 | 9 |
| MAGIC | 4 | 5 | 5 | 3 | 10 | 9 | 7 | 7 |
| BAMBI | 4 | 5 | 6 | 4 | 4 | 7 | 10 | 7 |
| TORRES | 10 | 3 | 7 | 7 | 4 | 8 | 4 | 5 |
| CHOMPA | 5 | 8 | 5 | 3 | 7 | 7 | 9 | 6 |
| MEAT | 9 | 2 | 5 | 8 | 6 | 8 | 6 | 4 |

## Planned Development Workflow

We will use the [ModRetro Chromatic Plugin](plugin://modretro-chromatic@openai-curated-remote) to author an editable native Game Boy Color project and produce a real cartridge ROM.

When implementation is requested, the intended workflow is:

1. Create and select the intended native `.gbsproj` project, preserving its `.gbsres` resources and authored assets.
2. Use the plugin's scene, event, collision, and asset tools; evaluate targeted extensions if the basketball mechanics require them.
3. Build and inspect the actual ROM with `rom_build` and `rom_inspect`.
4. Use `web_preview` in Codex's built-in browser and input-driven emulator checks to verify gameplay.
5. Verify cartridge saves and physical Chromatic behavior before claiming hardware readiness.

These are planned plugin operations, not shell commands configured in this repository. No build, test, or preview command is available yet. Document verified prerequisites and commands when implementation starts.

The read-only plugin readiness check on October 28, 2026 found no selected game project and an incompatible Node 20 runtime (the plugin requires a supported Node 22-or-newer runtime). GB Studio CLI, GBDK, and emulator dependencies were found through metadata; compilation and emulator startup have not been verified. Resolve the runtime binding before the first build.

The proposed first playable milestone is one scrolling court with four players, basic movement, possession, passing, shooting, and scoring. First verify that the native engine can handle the player, ball, and camera behavior at a usable frame rate. Placeholder uniforms and portraits can support that prototype while remaining art decisions are finalized.

## Planning Priorities & Open Decisions

- **Controls and rules:** map movement, shooting, passing, defense, and turbo to the handheld; decide quarter duration, possession rules, and tie handling.
- **Ratings and AI:** define each rating's effect, teammate behavior, and differences between the three CPU levels.
- **Presentation:** obtain player and uniform references; confirm CHOMPA's and MEAT's jersey numbers, court design, HUD, sound, and signature behavior.
- **Dunks and hot hand:** define Dunk Cam transitions, streak activation and reset conditions, and effects on play.
- **Hardware feasibility:** assess full-court readability, four-player animation, ball logic, camera effects, palette use, and performance within native Game Boy Color limits.
- **Persistent records:** choose tracked statistics, per-player attribution, save timing, storage layout, and reset behavior.

The ordered implementation roadmap is maintained in [PLAN.md](PLAN.md). This planning checkpoint does not create a game project or begin implementation.

## Repository & Contributions

- `README.md`: game brief, roster, planned workflow, and open decisions.
- `AGENTS.md`: contributor and agent instructions.
- `PLAN.md`: major feature milestones and acceptance checks.
- `.gitignore`: generated ROMs, build directories, local dependencies, environment files, emulator saves, and OS metadata. Authored art and native project resources remain tracked.
- `art/portraits/`: standalone portrait concepts, editable pixel data, and generation references. [MATUS v1](art/portraits/matus-v1/README.md) is approved as the portrait style reference; it has not been integrated into the game.
- [BAMBI v1](art/portraits/bambi-v1/README.md): approved portrait using the same size and palette; not yet integrated.
- [MAGIC v1](art/portraits/magic-v1/README.md): approved portrait with rectangular glasses and a broad smile, using the same size and palette; not yet integrated.
- [TORRES v1](art/portraits/torres-v1/README.md): approved portrait with short textured hair and a full beard, using the same size and palette; not yet integrated.
- [CHOMPA v2](art/portraits/chompa-v2/README.md): revised portrait based on the closer photograph, with a fuller jaw, chin stubble, and lime beanie; pending approval. Includes a four-color concept and a three-color variant. [V1](art/portraits/chompa-v1/README.md) remains available as an earlier, unapproved study.
- [MEAT v1](art/portraits/meat-v1/README.md): invented placeholder for the only roster player without a supplied photo; pending approval. Its appearance is provisional and must not be treated as a verified likeness.

See [Repository Guidelines](AGENTS.md) before contributing. During this phase, keep changes focused on planning, preserve the supplied roster and ratings, and clearly distinguish confirmed requirements from proposals.

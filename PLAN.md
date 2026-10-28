# Plan

Build **NICASOURCE JAM**, a native Game Boy Color game for ModRetro Chromatic, using the ModRetro Chromatic Plugin and an editable native project. One human controls a player alongside a CPU teammate against two CPU opponents. Start with a small playable basketball prototype, then complete the match flow, arcade features, presentation, and cartridge records.

This is a roadmap, not evidence of implemented gameplay. The repository currently contains planning documents and portrait concepts; no game project or playable ROM exists yet.

## Scope

- **In:** six crew members with the supplied ratings; player, teammate, and opponent selection; Warm-up, Pickup, and Winners Stay CPU levels; an angled scrolling court with 2D player sprites; four-quarter games; turbo, big dunks, a close-up Dunk Cam, hot-hand streaks and a flaming net; real team uniforms; per-player cartridge records.
- **First playable milestone:** one court, one human and three CPU players, movement in court depth and length, possession, passing, shooting, rebounds, and scoring. Temporary body sprites and uniforms are sufficient while proving the game mechanics.
- **Out:** multiplayer, additional rosters, and extra game modes for the initial release. Physical cartridge flashing remains a separate requested action.

## Action items

- [ ] **Establish the native build workflow.** Resolve the plugin's Node runtime mismatch, create and select the intended `.gbsproj`, and verify a genuine ROM build, cartridge inspection, emulator boot, and built-in browser preview. Record working prerequisites, versions, and output paths.
- [ ] **Prove court and runtime feasibility.** Prototype four independently moving players, court boundaries, a ball with separate ground position and flight height, depth ordering, and camera tracking. Measure native performance and sprite budgets. Prefer existing scene/events; evaluate a focused project-local engine extension if needed. Engine ejection requires a separate decision.
- [ ] **Implement possession and scoring.** Add dribbling, pass targets, catch/interception rules, shot arcs, two- and three-point scoring, rebounds, and possession resets. Verify loose balls, simultaneous catches, missed shots, and out-of-bounds recovery without softlocks.
- [ ] **Build the 2-on-2 CPU behavior.** Add teammate spacing, pass reception, shooting decisions, defense, rebounding, steals, and blocks. Implement the three CPU levels through reaction and decision tuning. Reach the first playable milestone with all four players active.
- [ ] **Add roster selection and ratings.** Let the user choose a controlled player, CPU teammate, two opponents, and difficulty. Integrate the portrait concepts with compatible native layouts. Preserve all supplied rating values; define and test their gameplay effects. Treat signature labels as flavor until their mechanics are specified.
- [ ] **Complete the match flow.** Add title/menu navigation, four quarters, clocks, intermissions, pause/resume, tie handling, results, and replay. Decide quarter duration and controller mapping through the prototype; avoid controls that require buttons unavailable on Game Boy Color.
- [ ] **Add the arcade features.** Implement turbo, exaggerated dunks, the close-up Dunk Cam, and hot-hand activation/reset with a flaming-net effect. Make camera transitions preserve possession, score, clocks, and player positions. Keep the basic game playable while effects are added.
- [ ] **Finish presentation.** Create readable body sprites and animations for movement, dribbling, passing, shooting, dunks, and defense; apply the real uniform references. Add court art, HUD, effects, and sound. Finalize pending portraits and retain MEAT's placeholder status until a likeness reference or explicit design approval is available.
- [ ] **Implement cartridge records.** Define win-loss attribution and the tracked statistics for each player, then add a versioned save format and results updates. Verify new cartridges, reloads, record resets, and invalid save data; avoid duplicate updates when reopening a results screen.
- [ ] **Validate and deliver the game.** Run repeatable gameplay checks on the exact ROM, including full matches, all CPU levels, overtime/ties, rebounds, interrupted camera sequences, and save/reload. Measure performance in crowded scenes and test on physical Chromatic before claiming hardware readiness. Deliver the tested ROM alongside its editable project.

## Open questions

- Which A/B/Select mapping and quarter duration feel best on the handheld? These should be settled during the first playable prototype.
- Which statistics should be saved, and how should wins/losses be credited to CPU teammates and opponents?
- What are the final uniform references and CHOMPA/MEAT portrait choices, and the missing CHOMPA/MEAT jersey numbers?

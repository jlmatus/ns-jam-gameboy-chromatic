# Plan

Build **NS JAM**, a native Game Boy Color game for ModRetro Chromatic, using the ModRetro Chromatic Plugin and an editable native project. One human controls a player alongside a CPU teammate against two CPU opponents. Start with a small playable basketball prototype, then complete the match flow, arcade features, presentation, and cartridge records.

The first playable prototype is implemented in `game/`: a native GBC-only ROM and official browser export. See `docs/playtests/first-playable.md` for verification. Unchecked milestones below still describe future work.

The second presentation pass addresses the user's first playtest: 16×32 bodies,
inward-facing baskets, a visible rim/net scoring sequence, animated nets, clearer
ball priority, and tighter court/camera spacing. See `docs/playtests/experience-v2.md`.
This is still a development prototype; the first rules build did not establish
finished-game feel or presentation quality.

## Current Milestone: Native Presentation Proof

Implemented the six-player native selection flow, approved portraits, unique lineups,
all supplied ratings, three CPU levels, lineup review, and return to selection from
pause. All 1,080 valid lineup/difficulty combinations pass host checks. Native ROM and
browser verification include selection, reviewed lineups, gameplay, pause, and
return to the picker. See `docs/playtests/selection-ratings.md` for evidence and
`docs/design/ratings.md` for formulas and controls.
The selected second layout is implemented: 64×64 portraits, bold type, larger
names, rating bars, and two detail pages. The user approved ratings-based
playstyle labels for all six players. See `docs/playtests/scouting-card-layout.md`.
The user accepted the cards. The first gameplay tuning pass reduces automatic
CPU takeaways and easy-mode opponent accuracy, protects catches, and gives the
existing body sprites side-facing profiles. See `docs/playtests/gameplay-balance.md`.
Configurable 1, 2 or 4 three-minute periods, period breaks and a basic final/replay stop are implemented; repeated one-minute overtime is implemented; a full-screen final summary, all four player box scores, and rematch/change-lineup are implemented; title navigation and richer quarter intermissions remain queued. Further tuning follows playtests.

The next user review requested better proportions, turning, shooting, and framing.
The latest candidate adds athletic 40-pixel bodies, front/back poses, a closer
native court projection, and staged shots with an attached ball and landing.
See `docs/playtests/athletic-motion.md`. Visual acceptance remains with the user.

The current contact pass uses the six confirmed heights/builds, positioning-based
human reaches, defensive shoves, power drives, gradual body separation, and
bounded recovery. CPU diagonal movement and retreat speed are corrected. See
`docs/playtests/body-contact.md` for exact-ROM evidence and acceptance limits.

The latest review adds catch-based control handoff, visible retrieval/baseline
inbounds, distinct player heads/skin palettes, and named jersey markers. See
`docs/playtests/identity-inbounds.md`.

The arcade finishing pass implements turbo + B dunks, rating-based takeoff/reach, exaggerated rise/slam/hang/landing, and a banked native Dunk Cam with zoom/wipe transitions. Makes return to baseline inbounds; misses and blocks return to live rebounds. See [design](docs/design/dunks.md) and [verification](docs/playtests/dunks.md). The turbo reserve now adds a visible gauge, depletion/recovery, exhaustion lockout and shared CPU/action costs; see `docs/design/turbo.md`. Hot-hand work remains separate.

The subsequent presentation pass adds articulated 24×56 dunk cels, one-hand/two-hand gathers, leg swing, rim rattle/net whip, crowd/impact effects and synchronized ball/camera publication. A cool slate court improves separation from skin and ball colors. See `docs/playtests/finishing-v2.md` for current captures and acceptance limits.

## Scope

- **In:** six crew members with the supplied ratings; player, teammate, and opponent selection; Warm-up, Pickup, and Winners Stay CPU levels; an angled scrolling court with 2D player sprites; configurable 1/2/4 three-minute periods; turbo, big dunks, a close-up Dunk Cam, hot-hand streaks and a flaming net; real team uniforms; per-player cartridge records.
- **First playable milestone:** one court, one human and three CPU players, movement in court depth and length, possession, passing, shooting, rebounds, and scoring. Temporary body sprites and uniforms are sufficient while proving the game mechanics.
- **Out:** multiplayer, additional rosters, and extra game modes for the initial release. Physical cartridge flashing remains a separate requested action.

## Confirmed Decisions

- **Brand and front end:** display name NS JAM; Night Court home revision 2 is approved and locked (2026-11-02). Preserve its composition, logo and palette. The intro still needs redesign; the full animated intro features MATUS, TORRES, BAMBI, CHOMPA and MAGIC, excluding MEAT. Review the revised cinematic before expanding all spotlights and cards.

- **Controls and timing:** use NBA Jam '99's default Game Boy Color mapping and configurable 1, 2 or 4 three-minute periods (four by default). The exact mapping and manual references are recorded in [README.md](README.md#confirmed-controls-and-timing).
- **Default portraits:** CHOMPA uses [v2](art/portraits/chompa-v2/README.md), and MEAT uses [v1](art/portraits/meat-v1/README.md). Both are approved defaults; MEAT's appearance is an accepted invented design.
- **Jerseys:** CHOMPA is `"74"`; MEAT is `"00"`. Store and render the numbers as strings to retain both zeros.
- **Persistent records:** defer the saved-statistics selection and win/loss attribution decisions until the complete match loop is playable. Track the live match score now; design cartridge persistence later.

## Action items

- [x] **Establish the native build workflow.** Managed Node 24 plugin session, editable `game/project.gbsproj`, official ROM build/header inspection, PyBoy boot, and built-in browser preview. Verified commands and dependency versions are in README.
- [x] **Prove prototype court and runtime feasibility.** Four independently moving players, bounded court, ball ground/height coordinates, depth-sorted render slots, and horizontal camera tracking run in the ROM. A project-local scene extension preserves the stock engine. The sampled crowded frame used ten OAM objects, with nine on its busiest scanline and no over-limit scanlines. Browser input advanced 72 frames during a 1.2-second hold. Broader performance and physical-hardware validation remain later work.
- [x] **Implement prototype possession and scoring.** Dribbling, flight/ground ball coordinates, fixed pass targets, deterministic catch arbitration, interceptions, charged shots, two/three-point scoring, rebounds, and possession resets. Player boundaries and bounded loose-ball recovery are implemented; made-basket retrieval and protected baseline inbounds are implemented; full out-of-bounds and foul rules remain future match work.
- [x] **Implement the basic 2-on-2 CPU behavior.** All four players are active: teammate spacing/pass reception/shooting, opponent marking/steals/shooting, rebounding, CPU shot blocking, and three selectable difficulty levels. Further balance and feel tuning remain later work.
- [x] **Add roster selection and ratings.** Native menus choose the human, teammate, two distinct opponents, and difficulty; review starts practice. Approved portraits and all six jersey strings are integrated. Every supplied rating is preserved and has a tested effect. See `docs/design/ratings.md`; signature labels remain flavor. The four-quarter clock is now implemented below.
- [x] **Implement arcade finishing.** Rating-based dunks, exaggerated jumps, native close-up Dunk Cam, entry/exit transitions, timed defensive contests, pause/resume and state-preserving returns to inbounds or live rebounds are implemented and verified. All six profiles and both baskets were exercised in the final native ROM; the official browser was played through MAGIC's dunk and return. See `docs/playtests/dunks.md` for captures and verification limits.
- [x] **Add the four-quarter clock.** Configurable 1/2/4 three-minute periods, setup/review confirmation, a fixed Q1–Q4/countdown HUD, hardware-time accounting, pause/Dunk Cam/inbound clock stops, released-shot resolution at zero, A-to-continue breaks, alternating period inbounds, and a basic final/replay stop are implemented. See `docs/playtests/quarters.md`.
- [ ] **Complete the remaining match flow.** Add richer quarter intermissions and finish the full intro. The NS JAM Night Court home/help/replay flow, larger MATUS card and one animated MATUS spotlight are integrated as a native-resolution proof; see `docs/playtests/front-proof.md`. Final results now show the winning pair, score and individual match box scores, with A rematch/B change-lineup; see `docs/playtests/final-results.md`. Repeated one-minute overtime, alternating inbounds and buzzer-shot tie resolution are implemented; see `docs/playtests/overtime.md`. SELECT pause/resume and basic replay are already implemented. Use A for pass/steal, B for shoot/block, and START for turbo; verify the original input feel through the prototype.
- [ ] **Finish the remaining arcade features.** Reserve-limited turbo sprint with a visible gauge, exaggerated dunks, Dunk Cam and state-preserving return transitions are implemented. Add hot-hand activation/reset with a flaming-net effect. Continue tuning from playtests.
- [ ] **Finish presentation.** Create readable body sprites and animations for movement, dribbling, passing, shooting, dunks, and defense; apply the real uniform references. Refine court art, HUD and effects. Native audio now implements 21 gameplay/menu/special-animation effects; rejected dunks have a dedicated animated BLOCKED stamp. See `docs/playtests/blocked-audio.md`. Integrate the approved default portraits and jerseys, including CHOMPA 74 and MEAT 00.
- [ ] **Validate the complete playable game.** Run repeatable checks on the exact ROM for full matches, all CPU levels, overtime/ties, rebounds, pause/resume, and interrupted camera sequences. Measure crowded-scene performance and test on physical Chromatic before claiming hardware readiness. Deliver a playable build with its editable project before designing saved records.
- [ ] **Add cartridge records later.** After the game is playable, revisit which statistics to retain and how wins/losses are credited to teammates and opponents. Then implement a versioned save format and verify new cartridges, reloads, record resets, invalid data, and duplicate results updates. The decisions and save schema do not block the playable game.

## Remaining Decisions

- Obtain the real team uniform references and finalize court/HUD presentation.
- Refine detailed possession rules and define signature behavior. Ties use repeated one-minute overtime. Rating formulas and initial difficulty behavior are implemented; continue tuning from playtests.
- Revisit saved statistics and record attribution after the complete game is playable, as explicitly deferred above.

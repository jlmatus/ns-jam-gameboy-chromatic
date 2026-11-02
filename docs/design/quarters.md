# Match clock and overtime

After CPU difficulty, MATCH LENGTH selects 1, 2 or 4 periods, each exactly 180 seconds. Four is the default; review confirms the choice and B returns to it. Each new lineup starts Q1 at 3:00. Periods retain the selected lineup, score and match statistics. The fixed HUD row shows `00:00 Q1 3:00 T` plus the existing four-cell turbo gauge. Player identity remains in the jersey marker, possession prompts and Dunk Cam background.

`jam_match.c` owns period state and timing. The renderer takes an atomic snapshot of GBDK `sys_time` every update and sends the unsigned VBlank delta to `jam_match_clock`. One hardware frame is exactly 1097/65536 seconds; a carried 16-bit fraction prevents drift and keeps timing independent of rendering/simulation throughput. There is no shortened development clock in the cartridge.

The clock stops during SELECT pause, Dunk Cam, dead-ball reset/retrieval, and the full inbound sequence through the protected pass. All clock deltas are sampled while stopped so resuming cannot charge paused time. It runs during live dribbling, ordinary passes, shots and rebounds.

At zero, new shots, passes and possessions are blocked. A released shot/finish can complete its existing rim sequence, scoring once if made. A miss ends the quarter before another possession. A shot still being charged is discarded. Pause remains available while a buzzer shot resolves.

Between the selected periods, A advances once; held buttons cannot skip multiple quarters. Scores/statistics remain, transient player/shot/contact state clears, and turbo refills. Q2/Q4 start with CPU inbounds and Q3 with human inbounds; the clock stays at 3:00 until the inbound catch. B returns to selection.

After the last configured period (Q1, Q2 or Q4), an untied score shows WIN or LOSE. A tie freezes on `TIED A OT B MENU`; A begins a 60-second overtime, and the HUD replaces Q1–Q4 with `OT`. Tied overtime periods repeat the same break until a period ends with a winner. This is a full timed period, with no sudden death. Released buzzer shots settle before comparing scores.

The selected regulation period count remains unchanged. `overtime` counts extra periods; the regulation quarter stays at Q1, Q2 or Q4. Inbounds alternate continuously from regulation through every overtime, and each new period refills turbo and clears transient animation/contact state while preserving score and match statistics. Overtime uses the same hardware clock, pause/Dunk Cam/dead-ball stops and protected inbound catch as regulation. One minute expires after 3,585 live VBlanks.

Scores use 16-bit storage, with a 9,999-point ceiling and two/three/four-digit HUD layouts. The former 99-point ceiling would have prevented breaking a 99–99 tie. The clock, period label and turbo gauge remain visible at every supported score width.

After a winner is decided, a full-screen final summary identifies the winning pair and score. Left/right cycles through the summary and four player box-score pages; A and B remain visible on every page. A starts a new Q1 with the same lineup, difficulty and period count, and cleared scores/statistics; B returns to selection. Returning to setup retains the previous period count for editing. Only 1, 2 and 4 are valid. A rematch also clears the overtime count. Quarter intermissions and cartridge records remain separate work. See `results.md` for stat definitions and final navigation. Native horn/start/win/lose sounds accompany the existing transitions.

# Four-quarter match clock

After CPU difficulty, MATCH LENGTH selects 1, 2 or 4 periods, each exactly 180 seconds. Four is the default; review confirms the choice and B returns to it. Each new lineup starts Q1 at 3:00. Periods retain the selected lineup, score and match statistics. The fixed HUD row shows `00:00 Q1 3:00 T` plus the existing four-cell turbo gauge. Player identity remains in the jersey marker, possession prompts and Dunk Cam background.

`jam_match.c` owns period state and timing. The renderer takes an atomic snapshot of GBDK `sys_time` every update and sends the unsigned VBlank delta to `jam_match_clock`. One hardware frame is exactly 1097/65536 seconds; a carried 16-bit fraction prevents drift and keeps timing independent of rendering/simulation throughput. There is no shortened development clock in the cartridge.

The clock stops during SELECT pause, Dunk Cam, dead-ball reset/retrieval, and the full inbound sequence through the protected pass. All clock deltas are sampled while stopped so resuming cannot charge paused time. It runs during live dribbling, ordinary passes, shots and rebounds.

At zero, new shots, passes and possessions are blocked. A released shot/finish can complete its existing rim sequence, scoring once if made. A miss ends the quarter before another possession. A shot still being charged is discarded. Pause remains available while a buzzer shot resolves.

Between the selected periods, A advances once; held buttons cannot skip multiple quarters. Scores/statistics remain, transient player/shot/contact state clears, and turbo refills. Q2/Q4 start with CPU inbounds and Q3 with human inbounds; the clock stays at 3:00 until the inbound catch. B returns to selection.

After the last configured period (Q1, Q2 or Q4), play freezes at the final score and shows WIN, LOSE or TIE. A starts a new Q1 with the same lineup, difficulty and period count, and cleared scores/statistics; B returns to selection. Returning to setup retains the previous period count for editing. Only 1, 2 and 4 are valid. Expanded results, audio, overtime, and cartridge records remain separate work.

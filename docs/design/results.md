# Final results and match box scores

After regulation/overtime produces a winner and every released buzzer shot has resolved, a full-screen navy results summary replaces the court. It shows both selected lineups, the final score, winning pair and overtime count. No result is shown for a tied overtime break.

Left/right cycles five pages: summary, your two players, then the two opponents. Each player page includes their name, string jersey (including MEAT `00`), team, winner marker and box score. The controls remain visible on every page: A rematches, B changes the lineup. Buttons held when the match ends cannot immediately skip results; navigation is edge-triggered and wraps. Final-screen input never updates simulation or consumes RNG. SELECT does not pause a frozen results page.

## Stat definitions

- **Points:** points actually added to the team score, credited to the shooter.
- **Field goals:** made/attempted shots, including dunks. A blocked/missed attempt remains an attempt.
- **3 pointers:** made/attempted threes, a subset of field goals.
- **Rebounds:** recovery of a missed/blocked shot or dunk. Pass failures and shove-fumble pickups do not count.
- **Steals:** successful dribble takeaways or pass interceptions, credited to the defender who gains possession.
- **Blocks:** credited to the successful defender, once per shot. Failed contests do not count.
- **Dunks:** made dunks, included in field goals and points.

`jam_player_stats_t stats[4]` belongs to lineup slots, so control handoff does not transfer statistics. Counters are 16-bit and accumulate across regulation, overtime, inbounds and pauses. `jam_stats_basket` awards score/individual makes exactly once. Aggregate simulation counters are retained for audio/diagnostics. Results render counters at fixed widths, clamping only the displayed value to 999 for shooting lines and 9,999 for other totals; ordinary short matches remain well below these limits.

`jam_results.c` owns pure tile generation and input navigation. `basketball.c` owns the full-window transition, native palette/font loading, sprite hiding and court restoration. `jam_menu_style` reuses the existing card palette/font with no new graphics tiles. Rematch uses normal lineup initialization, resets every stat and overtime counter, restores court/HUD and retains difficulty/length. Change-lineup returns to the existing selection flow with the previous setup available for editing.

Stats exist only in the live match. No cartridge record format or win/loss persistence is introduced. Assists and expanded quarter-break summaries are outside this pass.

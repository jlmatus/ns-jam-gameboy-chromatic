# Turbo reserve

Every lineup player starts a new practice with an independent 100-unit reserve. START/Enter plus a direction spends one unit per simulation update and keeps the existing +2 quarter-pixel movement boost. Standing with START held does not drain the meter. Charging/landing shots do not receive the sprint boost.

Release START to recover. Spending restarts a 20-update recovery delay; afterward the reserve gains two units per update, capped at 100. At zero, the player runs at normal speed and stays exhausted until reaching at least 25 units. Holding START at empty prevents recovery, so tiny automatic sprint bursts cannot occur. Walking without turbo can recover. SELECT pause freezes reserve, delay and exhaustion; Dunk Cam freezes them for every player. Baseline-inbound/dead-ball movement can recover even if START is held.

Turbo + B near the attacking rim costs 12 units for a dunk. Turbo + A costs eight units when committing to a nearby shove, including an attempt stopped by a stronger defender. With insufficient reserve, a dunk input retains the charged jump-shot fallback, and shoves are unavailable; normal A steals and B blocks still work. CPU power charges spend the same sprint reserve and CPU dunks pay the same finishing cost. Capacities and recovery are equal for all six players; no stamina rating or supplied-rating changes are introduced.

Reserves stay with the four lineup slots across possession/control changes and made baskets. Passing to a teammate shows that teammate's reserve, not a transferred or refilled pool. Starting a fresh lineup restores all four reserves.

## HUD and native resources

The top-right four-cell gauge has 16 fill steps. **T** means available; **X** means exhausted and recovering toward the 25-unit threshold. At empty, the HUD prompts RELEASE TURBO or TURBO RECOVERING when no higher-priority gameplay message is active. Normal play shows the controlled player's reserve; Dunk Cam shows the named dunker's reserve. Pause retains the gauge.

`jam_turbo.c` owns spending/recovery; `jam_game_t` appends three four-byte arrays so the existing player/world layout is preserved. `scripts/create-turbo-hud.mjs` generates five normal/opaque gauge tiles. Bank-zero font tiles 192–231 and gauge tiles 232–236 stay resident; cinematic transition art starts at 237, leaving 83 transition tiles across the two VRAM banks. Both normal and camera exits restore the intended gauge tiles. No OBJ allocation is added.

Verification and real captures: [turbo playtest](../playtests/turbo.md).

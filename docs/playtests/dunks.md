# Arcade finishing verification

## Implemented behavior

Hold START and press B near the attacking rim (**Enter + X** in the browser). The dunk rating controls takeoff range, jump height and finishing probability; body height and defensive block ratings affect contests. Outside dunk range, B retains the normal charged shot.

Dunks have gather, takeoff, cock-back, downward finish, rim hang and landing poses. The close-up uses native banked tile animation with a gradual zoom and entry/exit wipes. The ball stays attached to the action, crosses below the rim before two points are awarded, and can be blocked or miss. Makes return to visible baseline inbounds; failures return to live rebounds. SELECT/P pauses the sequence.

## Verification

- **Host rules:** `sh scripts/test.sh` passes with warnings as errors and address/undefined-behavior sanitizers. The 3,072 seeded dunk cases cover all six profiles at both baskets, range/depth rejection, turbo input and fallback shots, CPU finishing, exaggerated height, one score below the rim, misses, blocks, pause and state preservation. In 512 timed contests, MAGIC blocks 357 versus MATUS 113; supplied ratings are unchanged. Existing selection, shooting, contact and inbounds regressions pass. See [test output](dunks/host-tests.txt).
- **Exact native ROM:** `25f922d0fc14fd401490a66a9aaa999446d4886ecad334de06146d2c32ba5cd3`, built successfully by the official GB Studio CLI. Header inspection confirms a valid 2 MiB GBC-only cartridge. Known CLI deprecation and SDCC optimizer warnings remain; no build errors.
- **Genuine PyBoy 2.7.0 play:** recorded input-driven sequences exercise every profile. MAGIC, MATUS, CHOMPA and MEAT finish at the right basket; TORRES is visibly blocked by BAMBI with no score; CPU BAMBI finishes at the left basket. Makes return at 2:0 or 0:2 to retrieval/inbounds, and the block returns to a loose ball. Pause preserves all simulation bytes except previous input, including RNG; resume completes the dunk, and paused B returns to selection.
- **Native transitions:** all 4,096 inspected court-tile bytes across both VRAM banks match before and after MAGIC's camera sequence. Full retained intervals underpin the clips below. Double-buffered tile pages avoid partially drawn close-ups; opaque priority HUD tiles mask contesting sprites. The HUD fix was retested in the final native ROM and browser export. See [VRAM evidence](dunks/vram-evidence.json).
- **Official browser:** export ROM `6ae8c834fb764304bae178f317565e76c2f6a719182211d08df614c9ddf19cb8` opens in the existing built-in browser tab. MAGIC/MATUS versus BAMBI/TORRES on Warm-up was played with keyboard input. Enter + X starts Dunk Cam, P pauses/resumes, and the make returns to the court with score 02:00. The preview is left paused for review. Browser and native builds have distinct recorded binary identities.
- **Sprite allocation:** official source export plus `node scripts/check-native-sprite-budget.mjs` passes at 190/192 tiles per reserved bank span. Camera art uses at most 78 unique tiles per authored frame.

## Captures and limits

- [MAGIC: takeoff, finish and return](dunks/magic-dunk.gif)
- [TORRES: blocked finish and return](dunks/torres-block-dunk.gif)
- [BAMBI: CPU finish at the left basket](dunks/bambi-left-dunk.gif)
- [Browser Dunk Cam](dunks/browser-dunk-cam.png), [peak](dunks/browser-dunk-peak.png), [return](dunks/browser-return.png)
- [Build, runtime, branch, frame and digest receipts](dunks-evidence.json)

Clips are genuine retained emulator frames, with repeated images coalesced by the GIF exporter. Source pose previews in `art/dunks-v1/` are authored art, not playtest captures. Ordinary misses are verified in host simulation; the retained final native cases establish makes and a block. The cinematic zoom uses precomposed native frames rather than a hardware scale transform. This pass does not establish physical Chromatic performance or final game feel. Full matches, turbo meter, hot hand, final uniforms, sound and cartridge records remain separate work.

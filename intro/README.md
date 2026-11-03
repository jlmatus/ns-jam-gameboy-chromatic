# NS Jam intro: THE PLAY (v3)

A 27-second cold open that plays before the title screen. One possession in 14 hard cuts, no words on screen: the court lights come on, CHRUX cuts backdoor, Matus pulls from deep, Magic blocks it, Bambi dunks through the glass, Chompa answers, then all five slam onto the screen one per beat. It ends on the title screen, which already carries the NS Jam logo.

Not in it, on purpose: any text (no player names, no move names, no tagline), Meat, the studio name, a logo.

**Start here:** watch `reference/ns_jam_intro_reference.mp4` (60 fps, 4x, with the music). It is rendered by a simulator from exactly the data in this folder, so it is what the ROM should look like. `reference/storyboard.png` is the same on one page.

Everything is cooked. Do not redraw, recolour or re-fit anything. If something looks wrong in the emulator, compare against the reference video and report it.

---

## 1. What is in the folder

| Path | What it is |
|---|---|
| `gbstudio/assets/backgrounds/intro_*.png` | 14 backgrounds in GB Studio's 4 greens (what GB Studio imports) |
| `gbstudio/assets/backgrounds/intro_*.png.gbsres` | Their resource files: per-tile palette slot (`tileColors`), `autoColor: false`. Keep them next to the PNGs |
| `gbstudio/project/palettes/*.gbsres` | Palette resources with fixed UUIDs (`timeline.json` references them by id) |
| `gbstudio/assets/sprites/intro_ball.png` | Ball, 2 spin frames, 32x32 canvas (the ball is 22 px across, head-sized, as a real ball is) |
| `gbstudio/assets/sprites/intro_shard.png` | Backboard glass shard, 4 frames of 16x16 |
| `gbstudio/assets/music/ns_jam_intro.uge` | Music and all sound effects in one hUGETracker song (v6, 4 ticks/row, 120 BPM). 432 rows = 27.0 s, then 16 silent rows. It also carries the cut cues |
| `timeline.json` | Every shot: scene settings, the scene script, each actor's script, all as ordered steps |
| `validation.json` | Tile and palette counts per scene |
| `preview/` | Each background in its real colours (for eyeballing, not for import) |
| `reference/` | Reference video, its audio as WAV, storyboard |

## 2. Import

1. Copy `gbstudio/assets/*` into the project's `assets/` folders and `gbstudio/project/palettes/*` into `project/palettes/`. Open the project; GB Studio picks them up.
2. Backgrounds: leave **automatic colour off**. The colours come from `tileColors` (palette slot per 8x8 tile) plus the scene's 8 palettes.
3. Sprites `intro_ball` (canvas 32x32) and `intro_shard` (16x16): one animation state, frames as in the PNG. Sprite palettes: `intro sprite ball`, `intro sprite shard`.
4. Music `ns_jam_intro.uge`. Nothing else plays during the intro; the SFX are baked into the song.

## 3. Scenes

Create 14 scenes, one per shot, each named like its background (`intro_01_court` ... `intro_14_rollcall_5`). For each, read its entry in `timeline.json -> shots[]`:

| Setting | Value |
|---|---|
| Type | Top-down |
| Background | `background` |
| Background palettes slot 0-7 | `palette_ids` in order. This is the state the scene starts in (all black for the court, all white for the flash cuts) |
| Sprite palettes | slot 0 `intro sprite ball`, slot 1 `intro sprite shard` (only the scenes with those actors) |
| Player | Hidden (no sprite, or Actor Hide on the player as the first init event) |
| Camera | `camera_start`, set with Set Camera Position (pixels) at the top of init |
| Fades | Shot-to-shot cuts are instant: Switch Scene with fade Instant, scene auto fade-in Instant. The only fades are the `fade_out` steps in shots 6 and 14 |
| Actors | `actors[]`: sprite, palette, start position, "keep running while off-screen" on, collisions off, no auto-animation |

The 14 shots:

| # | Scene | Start (s) | Song row | What happens |
|---|---|---|---|---|
| 1 | intro_01_court | 0.0 | 0 | Black. Three clunks bring the court lights up, slow pan to the hoop |
| 2 | intro_02_chrux_face | 4.0 | 64 | CHRUX close-up, 2-frame white pop |
| 3 | intro_03_chrux_run | 5.0 | 80 | Whip pan in, rumble, whip out, the ball crosses |
| 4 | intro_04_matus | 9.0 | 144 | Tilt up from his sneakers to the ball leaving his fingertips, freeze-frame duotone, release |
| 5 | intro_05_magic | 12.0 | 192 | Camera tracks the ball up to Magic |
| 6 | intro_06_magic_block | 12.5 | 200 | The block: white flash, cracks, shake, the ball sent away, fade |
| 7 | intro_07_bambi_rise | 16.0 | 256 | Silhouette rises one step per thud |
| 8 | intro_08_bambi_dunk | 18.0 | 288 | Glass explodes (10 shards), the lights die |
| 9 | intro_09_chompa | 21.0 | 336 | Whip back, the hit lands with a shake |
| 10-14 | intro_10..14_rollcall_N | 23.0, 23.5, 24.0, 24.5, 25.0 | 368-400 | The five panels slam in one per half-beat; final flash and fade |

## 4. Sync: the music cuts the film

- Shot 1 init: **Play Music** `ns_jam_intro`.
- Every scene's init: **Set Music Routine 0** to a script that does **Switch Scene** to the next intro scene (Instant). The song fires routine 0 at each shot's start row (effect `6x0` on the wave channel, x = the shot number, so you can assert it if you want).
- Inside a shot, steps are timed in frames from the scene start. Because each shot starts on its cue, timing error never carries past one shot.
- Shot 14 ends with `end_intro`: Stop Music, Switch Scene to the title.
- **Skip:** A or START at any point: Stop Music, Switch Scene to the title. Attach it in every intro scene's init.

## 5. Script steps -> GB Studio events

Each shot has `script` (the scene's own script) and `actors[].script` (put each in that actor's On Init so actors run in parallel with the scene). The `at` field is the frame offset from the scene start, for checking.

| Step `op` | GB Studio event |
|---|---|
| `wait {frames}` | Wait, units frames |
| `camera_shake {frames, magnitude, direction}` | Camera Shake, units frames. It blocks the script, so long shakes are already split around the steps that happen during them |
| `set_background_palettes {slots}` | Set Background Palettes: the listed slots get those palette ids, every other slot Keep |
| `set_sprite_palettes {slots}` | Set Sprite Palettes, same rule (used for the ball during Matus's freeze-frame) |
| `camera_set_position {x, y}` | Set Camera Position, units pixels |
| `camera_move_to {x, y, speed_px_per_frame}` | Camera Move To, units pixels, speed 2 |
| `fade_out {frames}` | Fade Out, the speed closest to that many frames |
| `end_intro` | Stop Music, then Switch Scene to the title |
| actor `hide`, `show` | Actor Hide, Actor Show |
| actor `set_position {x, y}` | Actor Set Position, units pixels |
| actor `set_frame {frame}` | Actor Set Animation Frame |
| actor `move_to {x, y, speed_px_per_frame}` | Set Actor Movement Speed, then Actor Move To (pixels, diagonal, collisions off). Long moves are already split into short segments so they stay straight |

**Coordinates.** Every background has an 8 px dark border, so content (0,0) is scene (8,8); all numbers in `timeline.json` already include it. Camera values are the top-left of the 160x144 view, which is what GB Studio's camera events take. Actor positions are the top-left of the sprite canvas (32x32 ball, 16x16 shard). Calibrate once: place the ball, compare with the reference video, and if GB Studio draws it offset by a constant, add that constant to every actor position.

## 6. Why it is built this way (do not undo these)

- **8 px border on every background.** Camera Shake offsets the hardware scroll past the scene edge. The border makes the shake reveal dark, not stale VRAM.
- **White starts.** Scenes 2, 6, 8, 9 and 10-14 start all white and switch to their colours 2-4 frames in. Scene loads hide behind the flash. Scene 1 starts all black.
- **Ball scale.** 22 px across, about a head tall next to every player. Don't shrink it.
- **Matus.** Full body, athletic proportions, shown with a tilt from his sneakers up. The sprite is final.
- **Limits.** Max 243 unique tiles (the court), 8 palettes and 4 colours per tile everywhere, max 10 actors (the shards; they start at 5 different heights so no scanline starts with more than 8 sprites). The ball is 3 hardware sprites wide.

## 7. Check before calling it done

1. No text anywhere on screen.
2. No white or black frames other than the designed flashes and fades.
3. No fades between shots except after shots 6 and 14.
4. Cuts land on the music hits (side by side with the reference video).
5. Actor offset calibrated (section 5); the ball sits just off Matus's fingertips in shot 4.
6. A or START skips to the title.

Rebuild from source: `tools/intro/v2/build.sh` (Python 3 with numpy, pillow, scipy; ffmpeg).

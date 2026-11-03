# Dunk Cam scrambled tiles: stack overflow — 2026-11-03

## Symptom

Some matches showed Dunk Cam as scrambled tiles, and the HUD stayed garbled
for the rest of the match. With the human idle, the CHOMPA lineup reproduced it
deterministically: a CPU CHRUX dunk at frame ~5205 began cleanly and then broke
([capture](dunk-cam-stack/before-scrambled.png)). The previous commit's ROM
behaved identically, so this was not caused by the sprite-likeness change.

## Cause

Static RAM ended at `0xDE55`, and GB Studio fixes the stack top at `0xDF00`,
which left about 171 bytes of stack. Hooking function entries in PyBoy with the
build's `.noi` symbols showed the deepest path was a CPU dunk attempt
(`jam_try_dunk` → `jam_dunk_chance`/`jam_turbo_spend` →
`jam_turbo_available`, all banked calls) with the music/SFX VBlank interrupt
landing on top. Painting the stack showed it reaching `0xDE54`, which overwrote
GBDK's `_map_tile_offset`. After that, every `set_bkg_tiles`/`set_win_tiles`
call added the stray value to each byte (tiles +12, attributes `0x21`→`0x2d`),
so Dunk Cam and the HUD were drawn with shifted tile IDs. The VRAM tables and
art were correct, and only the written map bytes were offset.

## Fix

- The HUD buffers `hud[40]` and `previous_hud[40]` in
  `src/states/basketball.c` now alias the tail of `menu_tiles[360]`.
  `menu_tiles` is idle during play: menus and results rewrite all of it, the
  HUD is rebuilt in full on each draw, and both buffers are reset at tip-off.
- Removed the unused `jam_cam_row[20]`.
- Static RAM now ends at `0xDDF0`, giving the stack 101 more bytes.

## Validation

- Official CLI ROM build passes, SHA-256
  `9fca04092b7f225cfbdc62eb8802bcc8f5db8e4ef1d4a12e066a6bc5b8ebbda4`.
  The web export compiles.
- Stack painting from `0xDDF0` over 12,000 frames in each of six lineups
  (human = each roster player, idle): the deepest write was `0xDE35`, which
  leaves 69 bytes of headroom. `_map_tile_offset` stayed 0 in every run. On the
  old layout, three of those lineups would also have passed the end of static
  RAM.
- Five Dunk Cam events across the MAGIC and CHOMPA lineups, including the
  previously scrambled CHRUX dunk, render cleanly
  ([captures](dunk-cam-stack/after-dunks.png)).
- A full one-period match reaches the final results page, and **A Rematch**
  starts a clean match with a correct HUD and a clean BAMBI Dunk Cam
  ([sheet](dunk-cam-stack/after-full-match.png)).
- Sprite budget is 190/192, Dunk Cam is at most 123/128 tiles, and the
  sanitizer rules tests pass.
- Physical hardware is unverified. The headroom figure covers only these
  emulated runs, not a proven worst case.

`stack.py` (stack painting) and `dunkhunt.py` (Dunk Cam frame capture) are kept
in `dunk-cam-stack/` with their input scripts. Run them with the ModRetro
PyBoy venv: `python stack.py ROM INPUTS FRAMES DDF0`.

# Natural motion: side-view runs — 2026-11-04

## Problem

The exact cartridge was played in PyBoy, and each player was driven left,
right, up and down ([captures](natural-motion/court-runs.png)). Every body
pose drew the same front-facing head (both eyes, centered mouth) and a square,
front-on chest. Running left or right therefore showed a player facing the
camera while only the legs strode sideways. Running up or down shows one
standing pose flipped left/right, and the side run has only two frames.

## Change (`scripts/player-poses.mjs`)

The ready and stride poses (0–2) are shown only while a player faces left or
right, so they are now side-on:

- A profile head for each player, facing right (the sheet mirrors it for
  left): back of the head on the left, one eye, nose on the leading edge. The
  identity cues remain: MATUS spikes and goatee, MAGIC glasses, BAMBI's
  receding hairline and round face, CHRUX's crop and full beard, CHOMPA's
  beanie and glasses, and MEAT's textured hair.
- The chest is one pixel narrower per side; BAMBI (heavy) keeps a belly.
- Arms swing against the legs: in each stride the arm opposite the leading
  leg swings forward.
- No jersey number on side-on poses, because it is on the chest. Toward- and
  away-camera poses still show it.

Shots, blocks, contact and toward/away-camera poses keep the front face.
Dunk Cam requests the front head explicitly (`frontHead` in
`scripts/dunk-poses.mjs`), and its generated frames are byte-identical.

[Side poses for all six players](natural-motion/side-poses.png) ·
[before/after on court](natural-motion/before-after.png)

## Validation

- Crew sheet: 168 unique 8×16 tiles after x-flip (was 172). The loader
  check passes at **182/192** (was 186).
- Dunk Cam regenerated with no file changes (max 124/128 tiles).
- The official CLI ROM build passes, SHA-256
  `d6f49426d1e7b1262c74da65041e970fa5e522759be8ad9438398346a20ec54c`.
  Stack headroom is 528 bytes.
- Rules tests pass. No C code changed.
- Lineup inputs from `sprite-likeness/` were replayed with
  `natural-motion/motion.py` (and cropped with `crop.py`) on the old and new
  ROMs. Players face their running direction in profile.

## Not done

- Up/down runs still flip a standing pose; no up/down walking frames yet.
- The side run is still two frames.
- The profile faces need likeness approval. Browser and hardware play are
  unverified.

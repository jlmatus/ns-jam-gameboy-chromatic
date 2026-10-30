# Blocked dunk and sound verification

## Build and rules

- Official native build succeeds as a valid 4 MiB GBC-only MBC5 cartridge: SHA-256 `2e131016615e89c4fc6e6d79a23ca271382df3fd3d447008ead6ae7a015aaca2`. Existing CLI/SDCC optimizer warnings remain; the stock inline masked sound-cut helper also emits two constant-branch unreachable-code warnings. No errors.
- Sanitized host tests pass, including real pass/turbo/dunk event detection, one rejection/impact cue, no celebration after rejection, pause/resume and period/final cues, no simulation mutation, and all existing 3,240 setup combinations, clocks, shooting/contact/turbo tests. [Output](blocked-audio/host-tests.txt).
- All 504 generated camera frames fit their budgets (123 main maximum, 117 blocked maximum, 67 transition maximum). Official source export plus sprite check passes at 190/192 tiles; `git diff --check` passes.

## Actual cartridge playback

Genuine PyBoy 2.7.0 inputs verify:

- Right basket: MATUS is rejected by MAGIC. The large BLOCKED stamp animates, the rejected ball falls away, no points are awarded, and the court returns for a rebound (branch 1, frames 649–991).
- Left basket: human-controlled MAGIC times B against BAMBI's dunk. The stamp reads BLOCKED, not reversed text, with no scoring celebration; play returns to a rebound (branch 2, frames 945–1249).
- An uncontested BAMBI dunk has launch/slam cues, one two-point award and baseline inbound return, with no rejection stamp (branch 5, frames 945–1357).
- Audio diagnostic event IDs show rejection 16 and slam 15 at the intended transitions. The camera retains score, clock, gauge and pause behavior. Starting pause during an active rejection cuts effects; the captured last second of the paused game is entirely silent (peak 0, nonzero samples 0).

The actual cartridge's APU was captured through PyBoy at 48 kHz stereo. [Right-block route](blocked-audio-route.json), [slam route](slam-audio-route.json) and [pause route](pause-audio-route.json) reproduce the captures without memory writes. Every checked RGBA frame hash matches the corresponding public emulator samples. PCM receipts retain waveform hashes, peaks and RMS; raw WAVs are under ignored `game/build/blocked-audio/audio/`, with short genuine excerpts retained below. These are native APU output, not offline synthesized approximations.

## Browser and evidence

The official web export succeeds with ROM SHA-256 `08f77c4682a801a588a888957ff75564ef0c3df1f2f93669ce8e19fe14a51539`. Its new listener was opened in the existing built-in-browser tab after the expired old connection was identified. Native menus, match-length selection, movement, turbo and a blocked dunk were played in the browser; the speaker control is enabled. The preview is SELECT-paused on BLOCKED; P resumes. The earlier preview-update limitation is resolved for this build. Browser speaker output and physical hardware audio are not independently measured by the native PCM capture.

- [Browser screenshot](blocked-audio/browser-blocked.png)
- [Right rejection replay](blocked-audio/right-dunk.gif), [left rejection replay](blocked-audio/left-dunk.gif), [made dunk replay](blocked-audio/slam-dunk.gif)
- [Rejection sound excerpt](blocked-audio/blocked-demo.wav), [slam sound excerpt](blocked-audio/slam-demo.wav)
- [Native PCM receipts](blocked-audio/gameplay.json), [slam](blocked-audio/slam-gameplay.json), [pause](blocked-audio/pause-gameplay.json), [silence check](blocked-audio/pause-silence.json)
- [Build, native frame/state, recording and browser receipts](blocked-audio-evidence.json)

Replays contain genuine retained frames; duplicate images are coalesced, never interpolated. No physical Chromatic test or cartridge flash was performed. Background music and sampled announcer voices are separate from the implemented sound effects; final sound/visual tuning remains with user playtests.

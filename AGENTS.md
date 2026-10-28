# Repository Guidelines

## Project Scope & Current Phase

**NICASOURCE JAM** is a planned 2-on-2 arcade basketball game for ModRetro Chromatic, targeting Game Boy Color. Read `README.md` for the game brief, roster, and ratings, and `PLAN.md` for feature milestones and unresolved decisions.

Game development is currently **planning only**. Standalone player-face pixel-art exploration is authorized. Do not create a game project, install toolchains, build ROMs, or flash hardware until implementation is requested.

## Project Structure & Module Organization

The repository contains `README.md`, this guide, and standalone portrait studies in `art/portraits/`. No native game project, gameplay code, or tests exist yet. Preserve editable pixel data, generation prompts, and original artwork. Keep the README as the source of truth for the approved concept; distinguish proposals from confirmed requirements.

## ModRetro Chromatic Workflow

Use the **ModRetro Chromatic Plugin** for future implementation. Read its applicable authoring, pixel-art, or ROM-debugging skill before that work. Preserve editable native `.gbsproj` / `.gbsres` resources and keep authored assets inside the selected project. Prefer existing events and scene types; assess targeted extensions before considering engine ejection.

## Build, Test, and Development Commands

No repository build, test, lint, or preview commands are configured. Do not assume `npm test` or `make` exists. Once implementation begins, document verified commands and paths. Planned plugin tools include `rom_build`, `rom_inspect`, and `web_preview`; open plugin previews only in Codex's built-in browser. Exclude generated output and caches from version control.

## Style & Game Data Conventions

Use concise Markdown and consistent table columns. Preserve uppercase player names, signature labels, and the supplied 1–10 ratings. Unknown jersey numbers remain `null` in structured data. Do not invent uniform colors, signature effects, or balance changes. Follow native project conventions when code is introduced; no formatter or linter is configured yet.

## Testing Guidelines

No test framework or coverage threshold exists. For documentation, verify all six roster entries, ratings, links, and planning status. Future gameplay changes require genuine ROM builds and input-driven emulator checks; record the ROM, environment, steps, and results. Cartridge persistence needs save/reload verification. Distinguish emulator evidence from physical-device checks.

## Commit & Pull Request Guidelines

History contains only `first commit`; no established convention exists. Use focused commits with imperative subjects, such as `Document player selection flow`. PRs should explain purpose, decisions, verification, and relevant issues. Include gameplay captures once visual changes exist, and identify checks that could not be run.

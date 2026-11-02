# Latest browser preview restored

The user requested the updated playable build after approving Night Court and
MATUS v4. The original listener remained unresolved on a recording-close
acknowledgement, with no connected players. Built-in browser inventory also
showed no remaining tabs. Its listener, saved state and recording history were
left intact; no process was killed, journal reset or pending command replayed.

A fresh managed ModRetro MCP session was started with the existing repository
launcher `sh scripts/chromatic.sh` under the prepared Node 24 runtime. Public
`project_select` selected the same `game/project.gbsproj`, and public
`web_preview` built into a separate `build/web-current` directory, preserving the
old listener's output. This official build exited zero and its new listener
reported open. Existing CLI DEP0190 deprecation warning remains.

The returned URL was opened visibly in Codex's built-in browser. Genuine browser
screens show the approved city/volcano Night Court; pressing A opens the approved
MATUS v4 card. The view is left running on that card and marked as a deliverable.
No physical cartridge installation was requested or performed.

- Browser ROM SHA-256: `7af1c6ebf403adb4ac5aed2840b3e7b77bbc5d65477f3a5cff859d54a5e6d9b3`.
- New listener: `7cac88d8420a31ff82f09dbdaf9083bb`.
- Build source revision: `97d4c8fbe4e893aa7fbb91266266ea622cafc0cde519251b6a1b49de13982d7a`.
- Current-chat managed MCP transport: exec session `21103` (initialized as
  `ns-jam-preview-refresh`); keep it alive while this preview is in use. The
  original attached MCP tools still point to the older listener. Send public
  status/input calls through the owning transport rather than the old binding.
- Full build/listener receipt: ignored `game/build/preview-current-receipt.json`.
- [Visible browser evidence](browser-refresh/matus-browser.png).

The new URL is process-local and expires if its owning session ends. The older
recording-close uncertainty is preserved separately; it has not been claimed
resolved by opening this new preview.

#!/bin/sh
# Start the installed plugin with its setup-owned compatible Node. No reinstallation.
set -eu
jam_repo=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
jam_setup_root=${GB_STUDIO_SETUP_ROOT:-"$HOME/Library/Application Support/modretro-chromatic"}
jam_node=${JAM_NODE:-"$jam_setup_root/node/darwin-arm64/node-v24.15.0-darwin-arm64/bin/node"}
jam_plugin=${JAM_CHROMATIC_PLUGIN:-"$HOME/.codex/plugins/cache/openai-curated-remote/modretro-chromatic/1.0.33+codex.distribution.66e8961d3ce30093ea3c9ee4"}
export GB_STUDIO_WORKSPACE_ROOT="$jam_repo"
exec "$jam_node" "$jam_plugin/scripts/start-mcp.mjs" "$@"

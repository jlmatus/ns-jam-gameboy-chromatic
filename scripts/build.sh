#!/bin/sh
# Use the setup-owned modern Node instead of an inherited Node 20 MCP binding.
set -eu
jam_repo=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
jam_setup_root=${GB_STUDIO_SETUP_ROOT:-"$HOME/Library/Application Support/modretro-chromatic"}
jam_node=${JAM_NODE:-"$jam_setup_root/node/darwin-arm64/node-v24.15.0-darwin-arm64/bin/node"}
jam_cli=${JAM_GB_STUDIO_CLI:-"$jam_setup_root/toolchain/.local/vendor/gb-studio/out/cli/gb-studio-cli.js"}
if [ ! -x "$jam_node" ] || [ ! -f "$jam_cli" ]; then
  echo 'Managed Node/GB Studio missing. Run the ModRetro setup skill for build dependencies.' >&2
  exit 1
fi
"$jam_node" -e 'if(Number(process.versions.node.split(".")[0])<22)process.exit(1); console.log("Build runtime: Node " + process.versions.node)'
case "${1:-rom}" in
  rom) jam_command=make:rom; jam_output="$jam_repo/game/build/rom/nicasource-jam.gbc" ;;
  web) jam_command=make:web; jam_output="$jam_repo/game/build/manual-web" ;;
  export) jam_command=export; jam_output="$jam_repo/game/build/source" ;;
  *) echo 'Usage: sh scripts/build.sh [rom|web|export]' >&2; exit 1 ;;
esac
mkdir -p "$jam_repo/game/build/rom"
cd "$jam_repo/game"
exec "$jam_node" "$jam_cli" "$jam_command" "$jam_repo/game/project.gbsproj" "$jam_output"

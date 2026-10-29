#!/bin/sh
set -eu
jam_repo=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$jam_repo"
mkdir -p build/tests
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -fsanitize=undefined,address \
  -Igame/plugins/basketball/engine/include tests/jam_sim_test.c \
  game/plugins/basketball/engine/src/core/jam_sim.c \
  game/plugins/basketball/engine/src/core/jam_contact.c \
  game/plugins/basketball/engine/src/core/jam_menu.c -o build/tests/jam-sim-test
exec build/tests/jam-sim-test

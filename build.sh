#!/usr/bin/env bash
set -eu

dewy_root="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"

cmake -S "$dewy_root" -B "$dewy_root/build" -DCMAKE_BUILD_TYPE=Release
cmake --build "$dewy_root/build" --parallel
ctest --test-dir "$dewy_root/build" --output-on-failure

printf '\nDewy is ready. Start it with: ./run.sh\n'

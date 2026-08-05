#!/usr/bin/env bash
set -eu

dewy_root="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
cd "$dewy_root/build"
exec ./Dewy "$@"

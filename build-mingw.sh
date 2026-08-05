#!/usr/bin/env bash
set -eu

if [[ "${MSYSTEM:-}" != "UCRT64" ]]; then
    printf 'This build must run in the MSYS2 UCRT64 environment.\n' >&2
    printf 'From Windows, double-click build-mingw.bat instead.\n' >&2
    exit 1
fi

dewy_root="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
dewy_build="$dewy_root/build-mingw"

pacman -S --needed --noconfirm \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-glfw \
    mingw-w64-ucrt-x86_64-glew

cmake -S "$dewy_root" -B "$dewy_build" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$dewy_build" --parallel
ctest --test-dir "$dewy_build" --output-on-failure

# Copy non-system UCRT64 DLLs beside Dewy.exe so it can be launched from
# Explorer or a normal Command Prompt without keeping MSYS2 on PATH.
while IFS= read -r runtime_dll; do
    cp -f "$runtime_dll" "$dewy_build/"
done < <(ldd "$dewy_build/Dewy.exe" | awk '$3 ~ /^\/ucrt64\/bin\// { print $3 }')

printf '\nDewy is ready. Start it with: run-mingw.bat\n'

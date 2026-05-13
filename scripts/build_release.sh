#!/usr/bin/env bash
# scripts/build_release.sh
# Build firmware and assemble release artifacts:
#   - <version>-merged.bin    (single-file flash at 0x0)
#   - <version>-firmware.bin  (app-only image at 0x10000)
#   - <version>-checksums.txt (SHA-256 sums)
# Output lands in dist/

set -euo pipefail

ENV="${1:-Esp32dev}"
BUILD_DIR=".pio/build/$ENV"
OUT_DIR="dist"

# Resolve version from git describe
VERSION="$(git describe --tags --dirty 2>/dev/null || echo unknown)"
if [[ "$VERSION" == "unknown" ]]; then
    echo "warn: not in a git repo or no tags found; using 'unknown' as version" >&2
fi

# Build first (no-op if everything up to date)
echo "==> Building $ENV"
pio run -e "$ENV"

# Sanity check build artifacts
for f in firmware.bin bootloader.bin partitions.bin; do
    if [[ ! -f "$BUILD_DIR/$f" ]]; then
        echo "error: missing $BUILD_DIR/$f after build" >&2
        exit 1
    fi
done

# Locate boot_app0.bin in the framework package (not in the build dir)
BOOT_APP0=$(find "$HOME/.platformio/packages/framework-arduinoespressif32" \
            -path "*/tools/partitions/boot_app0.bin" 2>/dev/null | head -1)
if [[ -z "$BOOT_APP0" ]]; then
    echo "error: boot_app0.bin not found in framework-arduinoespressif32 package" >&2
    exit 1
fi

# Locate esptool — prefer PATH, fall back to PlatformIO-bundled
if command -v esptool.py >/dev/null 2>&1; then
    ESPTOOL_CMD=(esptool.py)
elif [[ -f "$HOME/.platformio/packages/tool-esptoolpy/esptool.py" ]]; then
    ESPTOOL_CMD=(python3 "$HOME/.platformio/packages/tool-esptoolpy/esptool.py")
else
    echo "error: esptool.py not found on PATH or in tool-esptoolpy package" >&2
    exit 1
fi

mkdir -p "$OUT_DIR"

MERGED="$OUT_DIR/${VERSION}-merged.bin"
APPONLY="$OUT_DIR/${VERSION}-firmware.bin"
CHECKSUMS="$OUT_DIR/${VERSION}-checksums.txt"

echo "==> Merging into $MERGED"
"${ESPTOOL_CMD[@]}" --chip esp32 merge_bin -o "$MERGED" \
    --flash_mode dio --flash_freq 40m --flash_size 4MB \
    0x1000  "$BUILD_DIR/bootloader.bin" \
    0x8000  "$BUILD_DIR/partitions.bin" \
    0xe000  "$BOOT_APP0" \
    0x10000 "$BUILD_DIR/firmware.bin"

cp "$BUILD_DIR/firmware.bin" "$APPONLY"

echo "==> Generating checksums"
( cd "$OUT_DIR" && sha256sum \
    "$(basename "$MERGED")" \
    "$(basename "$APPONLY")" \
    > "$(basename "$CHECKSUMS")" )

echo ""
echo "Release artifacts:"
ls -lh "$MERGED" "$APPONLY" "$CHECKSUMS"
echo ""
echo "Flash with:"
echo "  esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash 0x0 $MERGED"
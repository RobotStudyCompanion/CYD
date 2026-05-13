#!/usr/bin/env bash
# icon_prep.sh — batch-prep PNG icons for GUIslice Image2C
#
# Reads one or more source PNGs, outputs bilevel white-on-transparent PNGs
# (PNG32 RGBA on disk to bypass Java decoder bug with 1-bit greyscale + tRNS),
# optionally generating Image2C-compatible 1bpp C arrays in the same pass.
#
# Output direction is the contract; input direction is auto-detected by mean
# brightness and inverted only when needed.
#
# Works with both ImageMagick v6 (convert/identify) and v7 (magick).
# C array generation requires python3 and pbm_to_gslc_c.py alongside this script.
#
# Usage:
#   ./icon_prep.sh [-s WxH] [-i auto|on|off] [-c] <file.png> [file.png ...]
#   ./icon_prep.sh -s 40x40 -c *.png
#
# Output:
#   ./prepped/<basename>_<WxH>px.png
#   ./prepped/<basename>_<WxH>px.c   (only with -c)
#
# Re-runs silently overwrite existing outputs.

set -euo pipefail

# --- defaults ---
OUT_DIR="prepped"
SIZE="40x40"          # default; override with -s
INVERT_MODE="auto"    # auto | on | off
THRESHOLD="50%"       # b/w split point; lower = thinner strokes, higher = fatter
EMIT_C=false          # -c flag: also emit Image2C-format .c file

usage() {
    cat <<EOF
Usage: $0 [-s WxH] [-i auto|on|off] [-c] [-h] <file.png> [file.png ...]

  -s WxH    Resize to WxH pixels. Default: 40x40.
  -i MODE   Invert behaviour:
              auto  detect from mean brightness (default)
              on    force invert
              off   skip invert
  -c        Also emit C array (.c file) for direct firmware use.
            Requires python3 + pbm_to_gslc_c.py in script directory.
  -h        Show this help.

Output: ./${OUT_DIR}/<basename>_<WxH>px.png  (+ .c with -c)
EOF
    exit "${1:-0}"
}

# --- args ---
while getopts ":s:i:ch" opt; do
    case "$opt" in
        s)  SIZE="$OPTARG" ;;
        i)  INVERT_MODE="$OPTARG" ;;
        c)  EMIT_C=true ;;
        h)  usage 0 ;;
        \?) echo "unknown flag: -$OPTARG" >&2; usage 1 ;;
        :)  echo "flag -$OPTARG needs an argument" >&2; usage 1 ;;
    esac
done
shift $((OPTIND - 1))

case "$INVERT_MODE" in
    auto|on|off) ;;
    *) echo "invalid -i mode: $INVERT_MODE (use auto|on|off)" >&2; exit 1 ;;
esac

[[ $# -ge 1 ]] || usage 1

# --- preflight: pick ImageMagick binary (v7 magick > v6 convert) ---
if command -v magick >/dev/null 2>&1; then
    IM_CONVERT=(magick)
    IM_IDENTIFY=(magick identify)
elif command -v convert >/dev/null 2>&1 && command -v identify >/dev/null 2>&1; then
    IM_CONVERT=(convert)
    IM_IDENTIFY=(identify)
else
    echo "error: ImageMagick not found." >&2
    echo "  v6 (convert/identify): sudo apt install imagemagick" >&2
    echo "  v7 (magick):           sudo snap install imagemagick" >&2
    exit 1
fi

# --- preflight for -c flag ---
if [[ "$EMIT_C" == true ]]; then
    command -v python3 >/dev/null 2>&1 \
        || { echo "error: python3 not found (needed for -c)" >&2; exit 1; }
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PY_SCRIPT="$SCRIPT_DIR/pbm_to_gslc_c.py"
    [[ -f "$PY_SCRIPT" ]] \
        || { echo "error: pbm_to_gslc_c.py not found at $PY_SCRIPT" >&2; exit 1; }
fi

mkdir -p "$OUT_DIR"

# --- process ---
for src in "$@"; do
    if [[ ! -f "$src" ]]; then
        echo "skip (not a file): $src"
        continue
    fi

    # effective size — flag wins; otherwise read source dimensions
    if [[ -n "$SIZE" ]]; then
        eff_size="$SIZE"
    else
        eff_size=$("${IM_IDENTIFY[@]}" -format "%wx%h" "$src")
    fi

    # decide invert direction
    case "$INVERT_MODE" in
        on)
            do_invert=true
            tag="forced→invert"
            ;;
        off)
            do_invert=false
            tag="forced→keep"
            ;;
        auto)
            mean_pct=$("${IM_CONVERT[@]}" "$src" \
                -background white -alpha remove -alpha off \
                -colorspace Gray \
                -format "%[fx:int(mean*100)]" info:)
            if (( mean_pct > 50 )); then
                do_invert=true
                tag="bright→invert"
            else
                do_invert=false
                tag="dark→keep"
            fi
            ;;
    esac

    # build output paths
    base=$(basename "$src")
    stem="${base%.*}"
    out="$OUT_DIR/${stem}_${eff_size}px.png"

    # PNG pipeline: PNG32 RGBA on disk dodges Java's 1-bit greyscale + tRNS bug
    args=("$src" -background white -alpha remove -alpha off)
    [[ -n "$SIZE" ]] && args+=(-resize "${SIZE}!")
    [[ "$do_invert" == true ]] && args+=(-negate)
    args+=(
        -threshold "$THRESHOLD"
        -type Bilevel
        -transparent black
        "PNG32:$out"
    )
    "${IM_CONVERT[@]}" "${args[@]}"
    echo "✓ $out  [$tag]"

    # optional C-array emission
    if [[ "$EMIT_C" == true ]]; then
        cfile="${out%.png}.c"
        cname="${stem}_${eff_size}px"
        "${IM_CONVERT[@]}" "$out" \
            -background black -alpha remove -alpha off \
            -negate \
            pbm:- | \
        python3 "$PY_SCRIPT" \
            --name "$cname" \
            --src "$(basename "$out")" \
            --fg FFFFFF \
            > "$cfile"
        echo "  → $cfile"
    fi
done

echo "done — $OUT_DIR/"

#!/usr/bin/env bash
set -euo pipefail

: "${WOKWI_CLI_TOKEN:?WOKWI_CLI_TOKEN is required}"

PIO="${PIO:-platformio}"
WOKWI_CLI="${WOKWI_CLI:-wokwi-cli}"
PYTHON="${PYTHON:-python3}"
SERIAL_LOG="${SERIAL_LOG:-/tmp/wokwi_runtime.txt}"
WOKWI_TIMEOUT_SECONDS="${WOKWI_TIMEOUT_SECONDS:-60}"
WOKWI_SIM_TIMEOUT_MS="${WOKWI_SIM_TIMEOUT_MS:-30000}"
ELF=".pio/build/esp32dev/firmware.elf"

command -v "$PIO" >/dev/null 2>&1 || {
    echo "PlatformIO not found: $PIO" >&2
    exit 1
}

command -v "$WOKWI_CLI" >/dev/null 2>&1 || {
    echo "wokwi-cli not found: $WOKWI_CLI" >&2
    exit 1
}

command -v "$PYTHON" >/dev/null 2>&1 || {
    echo "python3 not found: $PYTHON" >&2
    exit 1
}

if command -v gtimeout >/dev/null 2>&1; then
    TIMEOUT_BIN="gtimeout"
elif command -v timeout >/dev/null 2>&1; then
    TIMEOUT_BIN="timeout"
else
    echo "timeout/gtimeout not found" >&2
    exit 1
fi

"$PIO" run -e esp32dev

test -f "$ELF" || {
    echo "missing ELF: $ELF" >&2
    exit 1
}

"$WOKWI_CLI" lint .

rm -f "$SERIAL_LOG"

if ! "$TIMEOUT_BIN" "$WOKWI_TIMEOUT_SECONDS" "$WOKWI_CLI" . \
    --elf "$ELF" \
    --serial-log-file "$SERIAL_LOG" \
    --timeout "$WOKWI_SIM_TIMEOUT_MS" \
    --timeout-exit-code 0; then
    echo "Wokwi simulation command failed" >&2
    exit 1
fi

"$PYTHON" scripts/parse_wokwi_telemetry.py "$SERIAL_LOG"

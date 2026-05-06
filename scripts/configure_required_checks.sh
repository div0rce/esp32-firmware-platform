#!/usr/bin/env bash
set -euo pipefail

repo="${1:-div0rce/esp32-firmware-platform}"
branch="${2:-main}"

gh api "repos/${repo}/branches/${branch}/protection/required_status_checks" \
    --method PATCH \
    -F strict=true \
    -f 'contexts[]=firmware-build' \
    -f 'contexts[]=native-tests' \
    -f 'contexts[]=wokwi-runtime' \
    -f 'contexts[]=esp-idf-builds (rtos)' \
    -f 'contexts[]=esp-idf-builds (cooperative)'

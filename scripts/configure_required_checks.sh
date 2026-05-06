#!/usr/bin/env bash
set -euo pipefail

repo="${1:-div0rce/esp32-firmware-platform}"
branch="${2:-main}"

if ! gh api "repos/${repo}/branches/${branch}/protection" >/dev/null 2>&1; then
    cat >&2 <<EOF
Branch protection is not enabled for ${repo}:${branch}.
Enable branch protection first, then rerun this script to set the required CI checks.
EOF
    exit 1
fi

gh api "repos/${repo}/branches/${branch}/protection/required_status_checks" \
    --method PATCH \
    -F strict=true \
    -f 'contexts[]=firmware-build' \
    -f 'contexts[]=native-tests' \
    -f 'contexts[]=wokwi-runtime' \
    -f 'contexts[]=esp-idf-builds (rtos)' \
    -f 'contexts[]=esp-idf-builds (cooperative)'

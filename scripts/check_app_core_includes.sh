#!/usr/bin/env bash
set -euo pipefail

forbidden='Arduino\.h|freertos/|driver/|esp_timer\.h|esp_task_wdt\.h|esp_log\.h|esp_err\.h'
paths=(
  components/app_core
  src/fault_state.c
  src/sensor_math.c
  src/state_machine.c
  src/telemetry_format.c
  src/time_utils.c
  include/fault_state.h
  include/sensor_math.h
  include/state_machine.h
  include/telemetry_format.h
  include/time_utils.h
  include/sample_request.h
)

if grep -R -n -E "#include[ <\"]+(${forbidden})" "${paths[@]}"; then
    echo "Forbidden runtime dependency found in app_core boundary"
    exit 1
fi

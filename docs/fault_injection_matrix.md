# Firmware Fault-Injection Matrix

This matrix maps supported firmware fault paths to the way they can be injected
or observed and to the repository evidence that backs the behavior. It includes
only faults that are represented by current source, tests, documentation, or
existing committed bench artifacts.

| Fault | Injection Method | Expected Behavior | Evidence |
|---|---|---|---|
| ADC low fault | Provide an ADC value below `ADC_LOW_FAULT_ENTER` through sensor math or the host fake HAL. Existing bench evidence also drives the physical potentiometer near GND. | Firmware reports `ADC_OUT_OF_RANGE`, enters `SYSTEM_STATE_FAULT`, and drives fault telemetry. | `src/sensor_math.c`, `test/test_sensor_math/test_sensor_math.cpp`, `test/test_app_core/test_app_core.cpp`, `test/test_integration/test_integration.cpp`, `docs/hardware_fault_injection.md`, `artifacts/hardware/esp32_fault_injection_log.txt` |
| ADC high fault | Provide an ADC value above `ADC_HIGH_FAULT_ENTER` through sensor math or the host fake HAL. Existing bench evidence also drives the physical potentiometer near 3V3. | Firmware reports `ADC_OUT_OF_RANGE`, enters `SYSTEM_STATE_FAULT`, and drives fault telemetry. | `src/sensor_math.c`, `test/test_sensor_math/test_sensor_math.cpp`, `test/test_app_core/test_app_core.cpp`, `test/test_integration/test_integration.cpp`, `docs/hardware_fault_injection.md`, `artifacts/hardware/esp32_fault_injection_log.txt` |
| ADC read failure | Make `platform_adc_read_raw()` return `false` through a HAL implementation or fake HAL configuration. | App core treats the sample as invalid ADC data, records `FAULT_ADC_OUT_OF_RANGE`, and emits fault telemetry. | `components/app_core/src/app_core.cpp`, `components/app_core/include/platform_hal.h`, `test/fakes/fake_platform_hal.cpp`, `test/fakes/fake_platform_hal.h` |
| UART write failure observability | Configure the host fake HAL UART write path to fail. | The fake HAL records the failed write attempt while the app core continues execution and does not reinterpret telemetry loss as an ADC fault. | `test/fakes/fake_platform_hal.cpp`, `test/fakes/fake_platform_hal.h`, `test/test_app_core/test_app_core.cpp`, `components/app_core/src/app_core.cpp` |
| Sensor sample queue send failure | Fill or otherwise reject `sensor_sample_queue` so `xQueueSend(sensor_sample_queue, &sample, 0)` returns failure in the RTOS backend. | RTOS backend enters `FAULT_SENSOR_QUEUE_SEND_FAILED`. | `components/app_backends/src/app_rtos_backend.cpp`, `include/fault_state.h`, `test/test_fault_state/test_fault_state.cpp`, `docs/design.md` |
| Button queue send failure | Force button queue overflow, either through ISR queue pressure or stress-mode queue injection. | Fault task consumes the overflow flag and enters `FAULT_BUTTON_QUEUE_SEND_FAILED`. | `src/button.cpp`, `components/app_backends/src/app_rtos_backend.cpp`, `src/stress_mode.cpp`, `include/fault_state.h`, `test/test_fault_state/test_fault_state.cpp`, `docs/stress_mode.md` |
| Sample timer failure | Make sample timer posting overflow or make `sampling_timer_start(sample_request_queue)` fail. | Fault task or startup path enters `FAULT_SAMPLE_TIMER_FAILED`. | `src/sampling_timer.cpp`, `include/sampling_timer.h`, `components/app_backends/src/app_rtos_backend.cpp`, `include/fault_state.h`, `test/test_fault_state/test_fault_state.cpp`, `docs/stress_mode.md` |
| Stale-sample watchdog timeout | Advance time by `APP_WATCHDOG_TIMEOUT_MS` after a previous sample timestamp without a fresh sample. | Firmware fault logic treats stale sampling as `FAULT_WATCHDOG_TIMEOUT` and moves to fault state. | `components/app_backends/src/app_rtos_backend.cpp`, `test/test_integration/test_integration.cpp`, `test/test_scheduler_timing/test_scheduler_timing.cpp`, `docs/timing_model.md` |
| Manufacturing-style self-test failure | Fail one of the boot self-test prerequisites such as app-state initialization, watchdog initialization, queue creation, task creation, ADC sanity, or button registration. | Firmware emits a `SELF_TEST` telemetry line with failed fields and enters `FAULT_MANUFACTURING_SELF_TEST_FAILED` when the completed self-test fails. | `src/manufacturing_self_test.cpp`, `include/manufacturing_self_test.h`, `components/app_backends/src/app_rtos_backend.cpp`, `src/telemetry_format.c`, `test/test_telemetry_format/test_telemetry_format.cpp`, `test/test_fault_state/test_fault_state.cpp`, `docs/manufacturing_self_test.md` |
| ADC fault recovery by button | Start from `SYSTEM_STATE_FAULT`, return ADC input to a valid value, and inject a button press through the fake HAL or physical button path. | Recoverable fault state clears and returns to `SYSTEM_STATE_IDLE`; status LED is driven low by the app core path. | `components/app_core/src/app_core.cpp`, `test/test_app_core/test_app_core.cpp`, `test/test_scheduler_timing/test_scheduler_timing.cpp`, `test/test_state_machine/test_state_machine.cpp` |

## Evidence Boundary

This matrix documents software-injected and previously captured
hardware-observed fault behavior. It does not claim automotive safety coverage,
production diagnostic coverage, exhaustive hardware fault validation, physical
ADC accuracy, electrical safety validation, sensor calibration, board-level
correctness, field reliability, or production readiness.

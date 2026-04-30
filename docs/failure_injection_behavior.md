# Failure-Injection Behavior

Scope: this describes host-side native test behavior, not physical ESP32 hardware output.

The behavior is covered by native tests, not by captured board telemetry.

## Covered Path

```text
BOOT -> IDLE -> SAMPLE -> TRANSMIT -> FAULT -> RECOVERY -> IDLE
```

## Native Test Coverage

| Behavior | Native test |
|---|---|
| Fault overrides nominal state flow | `test_fault_overrides_any_state` |
| Fault recovery returns system toward nominal operation | `test_fault_recovery_path` |
| Low ADC threshold hysteresis avoids chatter | `test_adc_low_fault_hysteresis` |
| High ADC threshold hysteresis avoids chatter | `test_adc_high_fault_hysteresis` |

## Boundary

These tests exercise portable firmware logic under PlatformIO's native environment. They do not prove ESP32 boot behavior, GPIO behavior, ADC pin behavior, UART output, or watchdog reset behavior on physical hardware.

# ESP32 Firmware Platform

Simulation-ready ESP32 firmware architecture targeting PlatformIO, Arduino-ESP32, FreeRTOS task design, native-tested firmware logic, and CI-backed validation.

This project implements timer-driven ADC sampling, ISR-driven button events, queue-based task communication, UART telemetry formatting, explicit fault states, and watchdog logic for ESP32-class firmware.

Hardware validation is not claimed because no physical ESP32 board was available during development.

## Build

```bash
pio run -e esp32dev
```

## Native Tests

```bash
pio test -e native
```

## Wokwi Simulation

Build the firmware first, then open the project with Wokwi using the root `wokwi.toml` and `diagram.json` files.

```bash
pio run -e esp32dev
```

## Validation Status

| Claim | Status | Evidence |
|---|---:|---|
| PlatformIO ESP32 firmware builds | Verified | Local build + GitHub Actions |
| Native logic tests pass | Verified | Local tests + GitHub Actions |
| Wokwi simulation configured | Ready | `wokwi.toml`, `diagram.json` |
| Wokwi serial telemetry observed | Pending | No captured simulation output yet |
| Physical ESP32 hardware validation | Not available | No board |
| Oscilloscope / logic analyzer traces | Not available | No board |
| Real hardware bring-up | Not claimed | Correctly withheld |

Current validation is build, host-side logic tests, and Wokwi simulation readiness. Physical ESP32 hardware validation is not claimed because no board was available during development.

## Documentation

- `docs/architecture.md`
- `docs/wiring.md`
- `docs/simulation_plan.md`
- `docs/test_plan.md`
- `docs/validation_log.md`
- `docs/timing_notes.md`

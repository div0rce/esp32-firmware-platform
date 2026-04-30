# Timing Budget

This document defines timing intent and firmware scheduling constraints. It is not a measured ESP32 timing trace.

| Item | Design value | Constraint rationale | Evidence type |
|---|---:|---|---|
| Sensor sample interval | Configured/design target: 100 ms | Keeps ADC processing periodic and bounded | Design intent |
| Telemetry receive timeout | Configured/design target: 250 ms | Separates sampling cadence from reporting cadence | Design intent |
| Fault task cadence | Configured/design target: 50 ms | Keeps overflow and stale-sample checks frequent relative to the application watchdog timeout | Design intent |
| Application watchdog timeout | Configured/design target: 1000 ms | Bounds stale sampling before entering recoverable fault handling | Design intent |
| Button debounce interval | Configured/design target: 200 ms | Prevents repeated recovery events from button bounce | Design intent |
| ADC fault hysteresis window | Configured/design target: ADC threshold margin | Avoids threshold chatter near fault boundaries | Native test coverage |
| Native test execution | Host-side only | Exercises portable C behavior outside Arduino/HAL | Automated test |

## Boundary

The values above come from firmware configuration and native-testable policy. They are not measured ESP32 runtime timing values. Physical sampling cadence, UART timing, GPIO latency, watchdog reset behavior, and ADC noise behavior require board-level validation.

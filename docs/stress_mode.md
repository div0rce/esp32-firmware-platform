# Stress Mode

The `seeed_xiao_esp32s3_stress` PlatformIO environment builds an opt-in firmware
variant for exercising fault paths without additional debug hardware.

```bash
pio run -e seeed_xiao_esp32s3_stress
pio run -e seeed_xiao_esp32s3_stress -t upload
pio device monitor
```

Stress mode changes are compile-time only and are not enabled in the default
`seeed_xiao_esp32s3` firmware.

## Stress Behaviors

| Behavior | Purpose |
|---|---|
| `SAMPLE_PERIOD_MS=10` | Raises sample pressure against the normal 100 ms cadence |
| `STRESS_TELEMETRY_DELAY_MS=75` | Slows the UART owner task to create backlog pressure |
| Forced ADC fault window | Drives `ADC_OUT_OF_RANGE` for part of each stress cycle |
| Recovery event injection | Queues button-style recovery events during the recovery window |
| Button overflow flag injection | Exercises `BUTTON_QUEUE_SEND_FAILED` handling |
| Sampling timer overflow flag injection | Exercises `SAMPLE_TIMER_FAILED` handling |

## Expected Result

The serial monitor should show normal telemetry mixed with deliberate fault
telemetry. `ADC_OUT_OF_RANGE`, `BUTTON_QUEUE_SEND_FAILED`, and
`SAMPLE_TIMER_FAILED` are expected in this build. `task_wdt` resets are not
expected.

## Boundary

Stress mode is a firmware fault-path exerciser. It is not timing closure,
hardware validation, or a substitute for debugger, JTAG, or logic-analyzer
captures.

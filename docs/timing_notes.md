# Timing Notes

## Configured Values

| Parameter | Value |
|---|---:|
| Sample period | 100 ms |
| Cooperative loop sleep | 1 ms |
| Telemetry receive timeout | 250 ms |
| Fault task period | 50 ms |
| Button debounce | 200 ms |
| Application watchdog timeout | 1000 ms |
| ESP task watchdog timeout | 3 s |
| UART baud rate | 115200 |

## Observations

Current evidence includes selected GPIO trace timing from real hardware in `docs/hardware_oscilloscope_trace.md`.

The repository does not claim timing closure, worst-case interrupt latency, deterministic hard real-time behavior under all ESP-IDF system conditions, or logic-analyzer coverage.

## Pending Or Deferred

- Logic analyzer trace: unavailable with current hardware.
- JTAG timing/debug evidence: unavailable with current hardware.
- Physical long-run stability: explicitly deferred.

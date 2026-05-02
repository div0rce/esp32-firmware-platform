# Runtime Health Telemetry

The firmware emits a periodic `RUNTIME_HEALTH` telemetry line from `telemetry_task`.
This keeps UART ownership in one task while surfacing RTOS health data during
simulation or bench runs.

Example format:

```text
timestamp_ms=3000,event=RUNTIME_HEALTH,heap_free=212000,heap_min=198000,stack_sensor=720,stack_telemetry=740,stack_button=350,stack_fault=310,samples=30,telemetry=30,button_events=1,fault_checks=60,queue_sample_req=0,queue_sensor=1,queue_button=0
```

Reported fields:

| Field | Meaning |
|---|---|
| `heap_free` | Current ESP heap free bytes |
| `heap_min` | Minimum ESP heap free bytes since boot |
| `stack_*` | FreeRTOS stack high-water mark in words for each task |
| `samples` | Sensor samples read by `sensor_task` |
| `telemetry` | Telemetry packets printed by `telemetry_task` |
| `button_events` | Button events received by `button_task` |
| `fault_checks` | Fault monitor loop iterations |
| `queue_*` | Current queue depth at capture time |

This telemetry is diagnostic evidence, not timing closure. It does not report
queue high-water occupancy, current draw, or worst-case interrupt latency.

# Physical Oscilloscope GPIO Timing Trace

## Setup

- Target board: Seeed Studio XIAO ESP32S3
- Firmware environment: PlatformIO `seeed_xiao_esp32s3_trace`
- Scope: Rigol DS1054Z
- Capture type: oscilloscope GPIO timing trace
- Input condition: potentiometer near midpoint
- Screenshot artifact: `artifacts/hardware/oscilloscope_trace_midpoint.png`

## Trace Channels

| Scope channel | XIAO pin | ESP32S3 GPIO | Firmware signal |
|---|---:|---:|---|
| CH1 | D3 | GPIO4 | sample trace |
| CH2 | D4 | GPIO5 | telemetry trace |
| CH3 | D5 | GPIO6 | fault task trace |

CH3 was displayed at `200 mV/div` for visibility because the fault task trace pulse is narrow. CH1 and CH2 were displayed at `1 V/div`.

## Scope Configuration

- Horizontal scale: `10 ms/div`
- CH1 vertical scale: `1 V/div`
- CH2 vertical scale: `1 V/div`
- CH3 vertical scale: `200 mV/div`
- Trigger: rising edge on CH3
- Probe attenuation: 10X probes

## Commands

```bash
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3_trace
/tmp/esp32-pio-venv/bin/pio test -e native
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3_trace -t upload
```

A short accompanying serial sanity check was captured from `/dev/cu.usbmodem101` at 115200 baud after the oscilloscope screenshot.

## Observed Result

- CH3 fault task trace period measured approximately `50 ms` on the oscilloscope.
- CH1 and CH2 trace pulses were visible around the triggered task activity.
- Accompanying serial output was non-empty.
- `fault=NONE` appeared in the accompanying serial output.
- `task_wdt` was not observed in the accompanying serial output.
- `ADC_OUT_OF_RANGE` was not observed in the accompanying serial output.

## Boundary

This is selected GPIO timing trace evidence from a physical XIAO ESP32S3 bench setup. It is not a logic analyzer capture. It does not prove timing closure, production timing validation, electrical safety, physical ADC accuracy, sensor calibration, board-level correctness, field reliability, or production readiness.

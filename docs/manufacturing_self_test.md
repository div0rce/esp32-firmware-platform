# Boot-Time Manufacturing Self-Test

## Purpose

The firmware emits a boot-time `SELF_TEST` telemetry line after core initialization and ADC setup. The intent is to provide a simple manufacturing-style sanity check that can be observed over USB serial during flashing or bench bring-up.

This is not a production manufacturing test suite. It is a bounded boot diagnostic for the current XIAO ESP32S3 firmware prototype.

## Current Checks

| Check | Method | Pass condition |
|---|---|---|
| ADC raw sanity | Single `analogRead(ADC_PIN)` after `sensor_init()` | Raw value is within configured ADC bounds, `ADC_RAW_MIN..ADC_RAW_MAX` |
| Button GPIO register read | Direct ESP32 GPIO input register read for `BUTTON_PIN` | Register read succeeds for the configured pin |
| Button pin state readable | Captured boot-time GPIO input level | Button level is emitted as diagnostic data |
| App state initialization | `app_state_init()` result | State mutex and initial state setup succeed |
| Watchdog initialization | `watchdog_init()` result | ESP task watchdog init succeeds or is already initialized |
| Queue creation | `create_queues()` result | Sample, telemetry, and button queues are allocated |
| Task creation | `create_tasks()` result | Sensor, telemetry, button, and fault tasks are created |
| Firmware metadata | Compile-time metadata from `app_config.h` | Firmware name, version, and target are emitted |

The ADC check intentionally verifies the ADC read path returns a representable raw value. It does not require the potentiometer to be at midpoint and does not prove ADC accuracy or calibration.

The button GPIO check intentionally verifies that the configured button pin can be sampled through the ESP32 GPIO input register path. It records the boot-time logic level but does not require the button to be pressed or released.

## Telemetry Format

Passing example:

```text
timestamp_ms=42,event=SELF_TEST,fw=esp32-firmware-platform,version=dev,target=seeed_xiao_esp32s3,result=PASS,adc_raw=2050,adc=PASS,button_reg=PASS,button=1,app_state=PASS,watchdog=PASS,queues=PASS,tasks=PASS
```

Failing example:

```text
timestamp_ms=43,event=SELF_TEST,fw=esp32-firmware-platform,version=dev,target=seeed_xiao_esp32s3,result=FAIL,adc_raw=-1,adc=FAIL,button_reg=FAIL,button=0,app_state=PASS,watchdog=FAIL,queues=FAIL,tasks=FAIL
```

If the self-test fails, firmware enters `FAULT_MANUFACTURING_SELF_TEST_FAILED` and drives the status LED high.

## Validation

The self-test telemetry format is covered by native tests in `test/test_telemetry_format/test_telemetry_format.cpp`. The explicit fault string is covered by `test/test_fault_state/test_fault_state.cpp`.

## Boundary

This boot-time self-test is a firmware diagnostic. The GPIO register read is a low-level firmware check, not complete bare-metal board validation. This document does not prove production readiness, electrical safety, physical ADC accuracy, sensor calibration, board-level correctness, field reliability, or high-volume manufacturing coverage.

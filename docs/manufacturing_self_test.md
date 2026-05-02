# Boot-Time Manufacturing Self-Test

## Purpose

The firmware emits a boot-time `SELF_TEST` telemetry line after core initialization and ADC setup. The intent is to provide a simple manufacturing-style sanity check that can be observed over USB serial during flashing or bench bring-up.

This is not a production manufacturing test suite. It is a bounded boot diagnostic for the current XIAO ESP32S3 firmware prototype.

## Current Checks

| Check | Method | Pass condition |
|---|---|---|
| ADC raw sanity | Single `analogRead(ADC_PIN)` after `sensor_init()` | Raw value is within configured ADC bounds, `ADC_RAW_MIN..ADC_RAW_MAX` |

The check intentionally verifies the ADC read path returns a representable raw value. It does not require the potentiometer to be at midpoint and does not prove ADC accuracy or calibration.

## Telemetry Format

Passing example:

```text
timestamp_ms=42,event=SELF_TEST,result=PASS,adc_raw=2050,adc_raw_in_range=PASS
```

Failing example:

```text
timestamp_ms=43,event=SELF_TEST,result=FAIL,adc_raw=-1,adc_raw_in_range=FAIL
```

If the self-test fails, firmware enters `FAULT_MANUFACTURING_SELF_TEST_FAILED` and drives the status LED high.

## Validation

The self-test telemetry format is covered by native tests in `test/test_telemetry_format/test_telemetry_format.cpp`. The explicit fault string is covered by `test/test_fault_state/test_fault_state.cpp`.

## Boundary

This boot-time self-test is a firmware diagnostic. It does not prove production readiness, electrical safety, physical ADC accuracy, sensor calibration, board-level correctness, field reliability, or high-volume manufacturing coverage.

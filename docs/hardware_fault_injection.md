# Physical ADC Fault-Injection Evidence

## Setup

- Target board: Seeed Studio XIAO ESP32S3
- Build environment: PlatformIO `seeed_xiao_esp32s3`
- Capture source: PlatformIO USB serial monitor
- Baud: 115200
- Input method: manually rotated potentiometer wired to the ADC input

## Cases

| Case | Input condition | Expected behavior | Observed behavior |
|---|---|---|---|
| Low endpoint | Potentiometer near GND | `ADC_OUT_OF_RANGE` | observed with `adc_raw=0` |
| Midpoint | Potentiometer near midpoint | `fault=NONE` | observed with ADC readings near midscale |
| High endpoint | Potentiometer near 3V3 | `ADC_OUT_OF_RANGE` | observed with `adc_raw=4095` |

The midpoint case was captured after returning the potentiometer to midpoint and recovering the board from the prior fault condition. The committed artifact contains selected verbatim serial excerpts from each raw capture.

## Commands

```bash
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3
/tmp/esp32-pio-venv/bin/pio test -e native

mkdir -p /tmp/esp32-hardware-capture

rm -f /tmp/esp32-hardware-capture/adc_low_fault_raw.txt
/tmp/esp32-pio-venv/bin/pio device monitor -b 115200 \
  | tee /tmp/esp32-hardware-capture/adc_low_fault_raw.txt
test -s /tmp/esp32-hardware-capture/adc_low_fault_raw.txt
grep -n "ADC_OUT_OF_RANGE" /tmp/esp32-hardware-capture/adc_low_fault_raw.txt | head
! grep -ni "task_wdt" /tmp/esp32-hardware-capture/adc_low_fault_raw.txt

rm -f /tmp/esp32-hardware-capture/adc_midpoint_raw.txt
/tmp/esp32-pio-venv/bin/pio device monitor -b 115200 \
  | tee /tmp/esp32-hardware-capture/adc_midpoint_raw.txt
test -s /tmp/esp32-hardware-capture/adc_midpoint_raw.txt
grep -n "fault=NONE" /tmp/esp32-hardware-capture/adc_midpoint_raw.txt | head
! grep -ni "task_wdt" /tmp/esp32-hardware-capture/adc_midpoint_raw.txt
! grep -n "ADC_OUT_OF_RANGE" /tmp/esp32-hardware-capture/adc_midpoint_raw.txt

rm -f /tmp/esp32-hardware-capture/adc_high_fault_raw.txt
/tmp/esp32-pio-venv/bin/pio device monitor -b 115200 \
  | tee /tmp/esp32-hardware-capture/adc_high_fault_raw.txt
test -s /tmp/esp32-hardware-capture/adc_high_fault_raw.txt
grep -n "ADC_OUT_OF_RANGE" /tmp/esp32-hardware-capture/adc_high_fault_raw.txt | head
! grep -ni "task_wdt" /tmp/esp32-hardware-capture/adc_high_fault_raw.txt
```

## Artifact

- `artifacts/hardware/esp32_fault_injection_log.txt`

## Boundary

This is physical serial evidence for firmware fault behavior under manual ADC input changes. It does not prove ADC accuracy, electrical safety, sensor calibration, board-level correctness, field reliability, or production readiness.

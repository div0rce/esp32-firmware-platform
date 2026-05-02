# Physical XIAO ESP32S3 Bring-Up

## Setup

- Target board: Seeed Studio XIAO ESP32S3
- Build environment: PlatformIO `seeed_xiao_esp32s3`
- Firmware commit: `5916326139bde848312d9bf4eeaa58f3b8edafd8`
- Capture source: PlatformIO USB serial monitor
- Baud: 115200
- Input condition: potentiometer near midpoint, button not pressed

## Commands

```bash
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3
/tmp/esp32-pio-venv/bin/pio test -e native
/tmp/esp32-pio-venv/bin/pio run -e seeed_xiao_esp32s3 -t upload
mkdir -p /tmp/esp32-hardware-capture
rm -f /tmp/esp32-hardware-capture/esp32_serial_raw.txt
/tmp/esp32-pio-venv/bin/pio device monitor -b 115200 \
  | tee /tmp/esp32-hardware-capture/esp32_serial_raw.txt
test -s /tmp/esp32-hardware-capture/esp32_serial_raw.txt
grep -n "fault=NONE" /tmp/esp32-hardware-capture/esp32_serial_raw.txt | head
! grep -ni "task_wdt" /tmp/esp32-hardware-capture/esp32_serial_raw.txt
! grep -n "ADC_OUT_OF_RANGE" /tmp/esp32-hardware-capture/esp32_serial_raw.txt
```

## Observed Result

- `fault=NONE` observed in the physical serial capture.
- `task_wdt` not observed in the physical serial capture.
- `ADC_OUT_OF_RANGE` not observed during the midpoint run.
- ADC readings in the selected serial excerpt are approximately midscale for the wired potentiometer position.

The full raw capture was kept outside the repository at `/tmp/esp32-hardware-capture/esp32_serial_raw.txt`. The committed artifact contains a bounded excerpt of actual captured serial lines.

## Artifact

- `artifacts/hardware/esp32_serial_log.txt`

## Boundary

This is physical serial bring-up evidence only. It does not prove physical ADC accuracy, electrical safety, sensor calibration, board-level correctness, field reliability, or production readiness.

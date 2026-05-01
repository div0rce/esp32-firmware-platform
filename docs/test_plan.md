# Test Plan

## Local Build

```bash
pio run -e seeed_xiao_esp32s3
```

Expected result:

- XIAO ESP32S3 firmware compiles with pinned PlatformIO dependencies.

## Native Logic Tests

```bash
pio test -e native
```

Expected result:

- State and fault string conversion tests pass.
- ADC conversion and fault classification tests pass.
- Telemetry formatting tests pass.

## CI

GitHub Actions runs:

- `firmware-build`
- `native-tests`

Both checks are required before protected `main` can accept a merge.

## Hardware Status

No physical XIAO ESP32S3 hardware validation is claimed until a board run is captured.

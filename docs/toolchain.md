# Toolchain

The primary firmware implementation targets ESP-IDF. PlatformIO remains in the
repository for host-native Unity tests, the current Wokwi runtime check, the
XIAO ESP32S3 PlatformIO build path, and legacy Arduino compatibility paths.

Arduino-dependent files are outside the primary ESP-IDF app-core architecture.
They must not be treated as part of the portable `components/app_core` boundary.
That boundary is enforced separately by `scripts/check_app_core_includes.sh`.

## ESP-IDF

The ESP-IDF build is pinned to ESP-IDF `v5.2.5` for ESP32S3.

Use that exact version for local and CI builds so peripheral APIs, generated configuration, and size output remain comparable between machines. `idf.py` should be available on `PATH`.

Expected version check:

```bash
idf.py --version
```

```text
ESP-IDF v5.2.5
```

Build both execution models with separate build directories:

```bash
idf.py -B build-rtos -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.rtos" set-target esp32s3 build
idf.py -B build-cooperative -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.cooperative" set-target esp32s3 build
```

Useful size commands:

```bash
idf.py -B build-rtos size
idf.py -B build-rtos size-components
idf.py -B build-cooperative size
idf.py -B build-cooperative size-components
```

## PlatformIO

PlatformIO currently serves three roles:

- host-native Unity tests through `env:native`;
- Wokwi runtime build support through `env:esp32dev`;
- legacy Arduino-compatible firmware builds through `env:seeed_xiao_esp32s3`.

The Arduino-compatible `src/main.cpp` and `src/firmware_hal.cpp` files are
retained for that compatibility path. They do not define the primary ESP-IDF
architecture and should not be used to evaluate whether the app core is isolated
from runtime headers.

```bash
pio test -e native
pio run -e seeed_xiao_esp32s3
```

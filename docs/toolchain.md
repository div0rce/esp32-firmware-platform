# Toolchain

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

PlatformIO remains only as the current host/native test path and legacy Arduino build path until replaced by CMake/CTest or ESP-IDF host testing.

```bash
pio test -e native
pio run -e seeed_xiao_esp32s3
```

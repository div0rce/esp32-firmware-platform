# Toolchain

## ESP-IDF

The ESP-IDF build is pinned to ESP-IDF `v5.2.5` for ESP32S3.

Use that exact version for local and CI builds so peripheral APIs, generated
configuration, and size output remain comparable between machines.

```bash
idf.py --version
idf.py -B build-rtos set-target esp32s3 build
```

Expected `idf.py --version` output:

```text
ESP-IDF v5.2.5
```

## PlatformIO

PlatformIO remains the current host/native test runner until the host tests are
replaced by CMake/CTest or ESP-IDF host testing.

```bash
pio test -e native
```

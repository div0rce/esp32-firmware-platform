# Branch Protection

`main` should require every pull request CI job from `.github/workflows/platformio.yml`.

Required status check contexts:

| Context | Workflow job |
|---|---|
| `firmware-build` | PlatformIO ESP32S3 firmware build |
| `native-tests` | PlatformIO native Unity tests |
| `wokwi-runtime` | Wokwi runtime telemetry gate |
| `esp-idf-builds (rtos)` | ESP-IDF RTOS backend build |
| `esp-idf-builds (cooperative)` | ESP-IDF cooperative backend build |

The branch protection setting is a GitHub repository setting, not a normal source-controlled workflow field. After this PR is merged, apply the setting with:

```bash
scripts/configure_required_checks.sh
```

The script requires branch protection to already be enabled for `main`; it fails with a clear error if that prerequisite is missing. It preserves strict required status checks so pull requests must be up to date with `main`.

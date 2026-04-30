# Simulation Plan

## Setup

1. Build firmware:

```bash
pio run -e esp32dev
```

2. Open the project in Wokwi.
3. Run the simulation using `wokwi.toml` and `diagram.json`.

## Simulation Checks

- Serial telemetry appears after boot.
- Potentiometer movement changes `adc_raw` and `voltage_mv`.
- Out-of-range ADC values produce `ADC_OUT_OF_RANGE`.
- Button events can clear recoverable faults.
- Timer cadence follows `SAMPLE_PERIOD_MS` approximately.

## Artifact Policy

Screenshots or logs must be labeled by source:

- `simulation`
- `ci`
- `hardware`

No physical hardware artifact is included unless it was produced from a real ESP32 board.

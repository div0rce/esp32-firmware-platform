# Hardware Abstraction Boundary

The firmware has a narrow hardware abstraction boundary for code that would
change in a bare-metal port.

## Implemented Boundary

| Hardware concern | Current implementation | Caller examples |
|---|---|---|
| UART output | `firmware_hal::uart_*` wraps Arduino `Serial` | `telemetry.cpp` |
| ADC input | `firmware_hal::adc_*` wraps Arduino ADC calls | `sensor.cpp`, `manufacturing_self_test.cpp` |
| GPIO mode/output | `firmware_hal::gpio_*` wraps Arduino GPIO calls | `main.cpp`, `button.cpp` |
| GPIO input register read | `firmware_hal::gpio_read_input_register` wraps the ESP32 register helper | `manufacturing_self_test.cpp` |
| Monotonic milliseconds and sleep | `firmware_hal::monotonic_millis`, `firmware_hal::sleep_ms` wrap Arduino time calls | `main.cpp` |
| Periodic timer | `sampling_timer.cpp` owns the ESP timer wrapper | `main.cpp` |
| Task watchdog | `watchdog.cpp` owns the ESP task watchdog wrapper | `main.cpp`, task loops |

The application logic should call the boundary functions or existing timer and
watchdog wrappers instead of direct Arduino, ESP-IDF, or register APIs.

## Bare-Metal Port Shape

A bare-metal port would replace `src/firmware_hal.cpp`, `src/sampling_timer.cpp`,
and `src/watchdog.cpp` with MCU-specific implementations. The portable C logic
for state transitions, ADC scaling, hysteresis, fault naming, telemetry
formatting, and rollover-safe time comparisons should remain unchanged.

## Boundary

This is a source-level abstraction boundary, not a completed bare-metal port.
The current implementation still targets Arduino-ESP32 on ESP32S3 hardware.

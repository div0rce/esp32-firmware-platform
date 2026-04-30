# Bare-Metal Porting Notes

This project currently targets ESP32 through Arduino-ESP32/PlatformIO.

The purpose of this boundary is to keep platform-independent firmware behavior testable outside the vendor framework.

## Portable Logic

The following behavior is implemented as portable C logic:

- state and fault string conversion
- ADC scaling, moving average, and fault hysteresis
- explicit state-machine transitions
- telemetry packet formatting
- rollover-safe elapsed-time checks

## Hardware Integration To Replace

A bare-metal port would replace the current Arduino/ESP32 integration wrappers:

| Current integration | Bare-metal replacement |
|---|---|
| Arduino/ESP timer timebase | hardware timer tick |
| `Serial` telemetry | UART driver |
| GPIO button interrupt wrapper | register-backed GPIO input and interrupt handling |
| ESP task watchdog wrapper | MCU watchdog register/service routines |
| Arduino `analogRead()` wrapper | MCU ADC register/HAL access |

The state machine, fault classification, telemetry formatting, ADC scaling, hysteresis policy, and rollover-safe timing logic would remain portable C.

## Boundary

This repository does not implement a bare-metal port. It documents the porting boundary so the current Arduino-ESP32 integration layer is not mistaken for register-level firmware.

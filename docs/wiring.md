# Wiring

## Pin Map

| Signal | ESP32 Pin | Notes |
|---|---:|---|
| ADC input | GPIO34 | Potentiometer or analog input |
| Button input | GPIO25 | Pull-up input, active-low |
| Status LED | GPIO2 | Wokwi LED / common onboard LED pin |
| UART | USB serial | 115200 baud |

## Wokwi Circuit

The Wokwi circuit connects:

- Potentiometer signal to GPIO34.
- Pushbutton to GPIO25 and GND.
- LED and 220 ohm resistor from GPIO2 to GND.

This wiring describes the configured simulation circuit and intended ESP32 target wiring. It is not a physical board validation record.

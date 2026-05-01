# Wiring

## Pin Map

| Signal | XIAO ESP32C3 Pin | Notes |
|---|---:|---|
| ADC input | D2 / GPIO4 | Potentiometer wiper or analog input |
| Button input | D1 / GPIO3 | Pull-up input, active-low |
| Status LED | D10 / GPIO10 | External LED through 220 ohm resistor |
| UART | USB serial | 115200 baud |

## Physical XIAO ESP32C3 Circuit

The physical XIAO ESP32C3 circuit connects:

- Potentiometer outer pins to 3V3 and GND.
- Potentiometer center/wiper pin to D2 / GPIO4.
- Pushbutton between D1 / GPIO3 and GND.
- LED anode to D10 / GPIO10 through a 220 ohm resistor.
- LED cathode to GND.

Keep all GPIO input voltages at 3.3 V. Do not feed a XIAO ESP32C3 GPIO pin with 5 V.

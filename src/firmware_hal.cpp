#include "firmware_hal.h"

#include <Arduino.h>

#include "gpio_registers.h"

namespace firmware_hal {

void uart_begin(std::uint32_t baud_rate) {
    Serial.begin(baud_rate);
}

void uart_println(const char* line) {
    Serial.println(line);
}

void adc_configure_resolution(std::uint8_t bits) {
    analogReadResolution(bits);
}

int adc_read_raw(std::uint8_t pin) {
    return analogRead(pin);
}

void gpio_configure_output(std::uint8_t pin) {
    pinMode(pin, OUTPUT);
}

void gpio_configure_input_pullup(std::uint8_t pin) {
    pinMode(pin, INPUT_PULLUP);
}

void gpio_write_level(std::uint8_t pin, bool high) {
    digitalWrite(pin, high ? HIGH : LOW);
}

bool gpio_read_input_register(std::uint8_t pin, bool* level) {
    return gpio_input_level_from_register(pin, level);
}

void gpio_attach_falling_interrupt(std::uint8_t pin, InterruptHandler handler) {
    attachInterrupt(digitalPinToInterrupt(pin), handler, FALLING);
}

std::uint32_t monotonic_millis() {
    return static_cast<std::uint32_t>(millis());
}

void sleep_ms(std::uint32_t milliseconds) {
    delay(milliseconds);
}

}  // namespace firmware_hal

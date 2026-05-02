#pragma once

#include <cstdint>

namespace firmware_hal {

using InterruptHandler = void (*)();

void uart_begin(std::uint32_t baud_rate);
void uart_println(const char* line);

void adc_configure_resolution(std::uint8_t bits);
int adc_read_raw(std::uint8_t pin);

void gpio_configure_output(std::uint8_t pin);
void gpio_configure_input_pullup(std::uint8_t pin);
void gpio_write_level(std::uint8_t pin, bool high);
bool gpio_read_input_register(std::uint8_t pin, bool* level);
void gpio_attach_falling_interrupt(std::uint8_t pin, InterruptHandler handler);

std::uint32_t monotonic_millis();
void sleep_ms(std::uint32_t milliseconds);

}  // namespace firmware_hal

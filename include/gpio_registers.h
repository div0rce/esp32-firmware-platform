#pragma once

#include <cstdint>

bool gpio_input_level_from_register(std::uint8_t pin, bool* level);

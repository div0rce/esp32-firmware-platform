#include "gpio_registers.h"

#include <soc/gpio_struct.h>

bool gpio_input_level_from_register(std::uint8_t pin, bool* level) {
    if (level == nullptr || pin >= 40U) {
        return false;
    }

    if (pin < 32U) {
        *level = ((GPIO.in >> pin) & 0x1U) != 0U;
    } else {
        *level = ((GPIO.in1.data >> (pin - 32U)) & 0x1U) != 0U;
    }

    return true;
}

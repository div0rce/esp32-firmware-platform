#include "firmware_hal.h"

#include <cstdio>

#include "app_config.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

adc_oneshot_unit_handle_t adc_unit = nullptr;
adc_bitwidth_t adc_bitwidth = ADC_BITWIDTH_12;
firmware_hal::InterruptHandler gpio_interrupt_handler = nullptr;

bool adc_channel_for_pin(std::uint8_t pin, adc_channel_t* channel) {
    if (channel == nullptr) {
        return false;
    }

    if (pin >= 1U && pin <= 10U) {
        *channel = static_cast<adc_channel_t>(pin - 1U);
        return true;
    }

    return false;
}

adc_bitwidth_t adc_bitwidth_for_resolution(std::uint8_t bits) {
    switch (bits) {
        case 9:
            return ADC_BITWIDTH_9;
        case 10:
            return ADC_BITWIDTH_10;
        case 11:
            return ADC_BITWIDTH_11;
        case 12:
        default:
            return ADC_BITWIDTH_12;
    }
}

bool ensure_adc_unit() {
    if (adc_unit != nullptr) {
        return true;
    }

    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.unit_id = ADC_UNIT_1;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;

    return adc_oneshot_new_unit(&init_config, &adc_unit) == ESP_OK;
}

void IRAM_ATTR gpio_isr_trampoline(void*) {
    if (gpio_interrupt_handler != nullptr) {
        gpio_interrupt_handler();
    }
}

}

namespace firmware_hal {

void uart_begin(std::uint32_t) {
    setvbuf(stdout, nullptr, _IONBF, 0);
}

void uart_println(const char* line) {
    if (line == nullptr) {
        return;
    }

    std::puts(line);
}

void adc_configure_resolution(std::uint8_t bits) {
    adc_bitwidth = adc_bitwidth_for_resolution(bits);

    adc_channel_t channel;
    if (!adc_channel_for_pin(ADC_PIN, &channel) || !ensure_adc_unit()) {
        return;
    }

    adc_oneshot_chan_cfg_t channel_config = {};
    channel_config.bitwidth = adc_bitwidth;
    channel_config.atten = ADC_ATTEN_DB_12;
    (void)adc_oneshot_config_channel(adc_unit, channel, &channel_config);
}

int adc_read_raw(std::uint8_t pin) {
    adc_channel_t channel;
    if (!adc_channel_for_pin(pin, &channel) || !ensure_adc_unit()) {
        return -1;
    }

    int raw = -1;
    if (adc_oneshot_read(adc_unit, channel, &raw) != ESP_OK) {
        return -1;
    }

    return raw;
}

void gpio_configure_output(std::uint8_t pin) {
    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << pin;
    config.mode = GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    (void)gpio_config(&config);
}

void gpio_configure_input_pullup(std::uint8_t pin) {
    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << pin;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    (void)gpio_config(&config);
}

void gpio_write_level(std::uint8_t pin, bool high) {
    (void)gpio_set_level(static_cast<gpio_num_t>(pin), high ? 1 : 0);
}

bool gpio_read_input_register(std::uint8_t pin, bool* level) {
    if (level == nullptr) {
        return false;
    }

    *level = gpio_get_level(static_cast<gpio_num_t>(pin)) != 0;
    return true;
}

void gpio_attach_falling_interrupt(std::uint8_t pin, InterruptHandler handler) {
    gpio_interrupt_handler = handler;

    const esp_err_t install_result = gpio_install_isr_service(0);
    if (install_result != ESP_OK && install_result != ESP_ERR_INVALID_STATE) {
        return;
    }

    (void)gpio_set_intr_type(static_cast<gpio_num_t>(pin), GPIO_INTR_NEGEDGE);
    (void)gpio_isr_handler_add(static_cast<gpio_num_t>(pin), gpio_isr_trampoline, nullptr);
}

std::uint32_t monotonic_millis() {
    return static_cast<std::uint32_t>(esp_timer_get_time() / 1000LL);
}

void sleep_ms(std::uint32_t milliseconds) {
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
}

}  // namespace firmware_hal

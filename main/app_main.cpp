#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr const char* kBootTag = "boot";

}

extern "C" void app_main(void) {
    ESP_LOGI(kBootTag, "esp32-firmware-platform ESP-IDF scaffold booted");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

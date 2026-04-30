#include "watchdog.h"

#include <esp_err.h>
#include <esp_idf_version.h>
#include <esp_task_wdt.h>

#include "app_config.h"

static bool is_success_or_already_initialized(esp_err_t result) {
    return result == ESP_OK || result == ESP_ERR_INVALID_STATE;
}

bool watchdog_init() {
#if ESP_IDF_VERSION_MAJOR >= 5
    esp_task_wdt_config_t config = {};
    config.timeout_ms = TASK_WATCHDOG_TIMEOUT_SECONDS * 1000U;
    config.idle_core_mask = 0;
    config.trigger_panic = true;

    return is_success_or_already_initialized(esp_task_wdt_init(&config));
#else
    return is_success_or_already_initialized(
        esp_task_wdt_init(TASK_WATCHDOG_TIMEOUT_SECONDS, true)
    );
#endif
}

bool watchdog_register_current_task() {
    return esp_task_wdt_add(nullptr) == ESP_OK;
}

void watchdog_reset_current_task() {
    (void)esp_task_wdt_reset();
}

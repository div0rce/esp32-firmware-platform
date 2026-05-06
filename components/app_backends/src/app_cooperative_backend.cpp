#include "app_cooperative_backend.h"

#include <cstdint>

#include "app_core.h"
#include "platform_hal.h"

void app_cooperative_start() {
    if (!platform_init()) {
        while (true) {
            platform_time_sleep_ms(1000);
        }
    }

    AppCoreContext context;
    app_core_init(&context);

    while (true) {
        const uint32_t now_ms = platform_time_now_ms();
        platform_trace_set(PLATFORM_TRACE_LOOP, true);
        app_core_tick(&context, now_ms);
        platform_trace_set(PLATFORM_TRACE_LOOP, false);
        platform_watchdog_kick();
        platform_time_sleep_ms(1);
    }
}

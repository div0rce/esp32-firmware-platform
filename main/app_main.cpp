#include "sdkconfig.h"

#include "app_cooperative_backend.h"
#include "app_rtos_backend.h"

extern "C" void app_main(void) {
#if CONFIG_APP_EXECUTION_MODEL_RTOS
    app_rtos_start();
#elif CONFIG_APP_EXECUTION_MODEL_COOPERATIVE
    app_cooperative_start();
#else
#error "No execution model selected"
#endif
}

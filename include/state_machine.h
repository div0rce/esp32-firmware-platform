#pragma once

#include "fault_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APP_EVENT_BOOT_COMPLETE = 0,
    APP_EVENT_SAMPLE_STARTED,
    APP_EVENT_TRANSMIT_STARTED,
    APP_EVENT_TRANSMIT_COMPLETE,
    APP_EVENT_FAULT_DETECTED,
    APP_EVENT_RECOVERY_REQUESTED,
    APP_EVENT_RECOVERY_COMPLETE
} AppEvent;

SystemState state_machine_next(SystemState current_state, AppEvent event);

#ifdef __cplusplus
}
#endif

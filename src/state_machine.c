#include "state_machine.h"

SystemState state_machine_next(SystemState current_state, AppEvent event) {
    if (event == APP_EVENT_FAULT_DETECTED) {
        return SYSTEM_STATE_FAULT;
    }

    switch (current_state) {
        case SYSTEM_STATE_BOOT:
            return event == APP_EVENT_BOOT_COMPLETE ? SYSTEM_STATE_IDLE : current_state;

        case SYSTEM_STATE_IDLE:
            return event == APP_EVENT_SAMPLE_STARTED ? SYSTEM_STATE_SAMPLE : current_state;

        case SYSTEM_STATE_SAMPLE:
            return event == APP_EVENT_TRANSMIT_STARTED ? SYSTEM_STATE_TRANSMIT : current_state;

        case SYSTEM_STATE_TRANSMIT:
            return event == APP_EVENT_TRANSMIT_COMPLETE ? SYSTEM_STATE_IDLE : current_state;

        case SYSTEM_STATE_FAULT:
            return event == APP_EVENT_RECOVERY_REQUESTED ? SYSTEM_STATE_RECOVERY : current_state;

        case SYSTEM_STATE_RECOVERY:
            return event == APP_EVENT_RECOVERY_COMPLETE ? SYSTEM_STATE_IDLE : current_state;

        default:
            return current_state;
    }
}

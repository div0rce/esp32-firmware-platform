#include "fault_state.h"

const char* state_to_string(SystemState state) {
    switch (state) {
        case SYSTEM_STATE_BOOT:
            return "BOOT";
        case SYSTEM_STATE_IDLE:
            return "IDLE";
        case SYSTEM_STATE_SAMPLE:
            return "SAMPLE";
        case SYSTEM_STATE_TRANSMIT:
            return "TRANSMIT";
        case SYSTEM_STATE_FAULT:
            return "FAULT";
        case SYSTEM_STATE_RECOVERY:
            return "RECOVERY";
        default:
            return "UNKNOWN";
    }
}

const char* fault_to_string(FaultCode fault) {
    switch (fault) {
        case FAULT_NONE:
            return "NONE";
        case FAULT_ADC_OUT_OF_RANGE:
            return "ADC_OUT_OF_RANGE";
        case FAULT_SENSOR_QUEUE_SEND_FAILED:
            return "SENSOR_QUEUE_SEND_FAILED";
        case FAULT_BUTTON_QUEUE_SEND_FAILED:
            return "BUTTON_QUEUE_SEND_FAILED";
        case FAULT_SAMPLE_TIMER_FAILED:
            return "SAMPLE_TIMER_FAILED";
        case FAULT_WATCHDOG_TIMEOUT:
            return "WATCHDOG_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}

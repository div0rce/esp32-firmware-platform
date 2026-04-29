#include "fault_state.h"

const char* state_to_string(SystemState state) {
    switch (state) {
        case SystemState::BOOT:
            return "BOOT";
        case SystemState::IDLE:
            return "IDLE";
        case SystemState::SAMPLE:
            return "SAMPLE";
        case SystemState::TRANSMIT:
            return "TRANSMIT";
        case SystemState::FAULT:
            return "FAULT";
        case SystemState::RECOVERY:
            return "RECOVERY";
        default:
            return "UNKNOWN";
    }
}

const char* fault_to_string(FaultCode fault) {
    switch (fault) {
        case FaultCode::NONE:
            return "NONE";
        case FaultCode::ADC_OUT_OF_RANGE:
            return "ADC_OUT_OF_RANGE";
        case FaultCode::SENSOR_QUEUE_SEND_FAILED:
            return "SENSOR_QUEUE_SEND_FAILED";
        case FaultCode::BUTTON_QUEUE_SEND_FAILED:
            return "BUTTON_QUEUE_SEND_FAILED";
        case FaultCode::SAMPLE_TIMER_FAILED:
            return "SAMPLE_TIMER_FAILED";
        case FaultCode::WATCHDOG_TIMEOUT:
            return "WATCHDOG_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}

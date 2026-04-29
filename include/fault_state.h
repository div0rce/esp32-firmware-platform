#pragma once

enum class SystemState {
    BOOT,
    IDLE,
    SAMPLE,
    TRANSMIT,
    FAULT,
    RECOVERY
};

enum class FaultCode {
    NONE,
    ADC_OUT_OF_RANGE,
    SENSOR_QUEUE_SEND_FAILED,
    BUTTON_QUEUE_SEND_FAILED,
    SAMPLE_TIMER_FAILED,
    WATCHDOG_TIMEOUT
};

const char* state_to_string(SystemState state);
const char* fault_to_string(FaultCode fault);

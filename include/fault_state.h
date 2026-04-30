#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SYSTEM_STATE_BOOT = 0,
    SYSTEM_STATE_IDLE,
    SYSTEM_STATE_SAMPLE,
    SYSTEM_STATE_TRANSMIT,
    SYSTEM_STATE_FAULT,
    SYSTEM_STATE_RECOVERY
} SystemState;

typedef enum {
    FAULT_NONE = 0,
    FAULT_ADC_OUT_OF_RANGE,
    FAULT_SENSOR_QUEUE_SEND_FAILED,
    FAULT_BUTTON_QUEUE_SEND_FAILED,
    FAULT_SAMPLE_TIMER_FAILED,
    FAULT_WATCHDOG_TIMEOUT
} FaultCode;

const char* state_to_string(SystemState state);
const char* fault_to_string(FaultCode fault);

#ifdef __cplusplus
}
#endif

#include "app_state.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

static SemaphoreHandle_t state_mutex = nullptr;
static SystemState current_state = SYSTEM_STATE_BOOT;
static FaultCode current_fault = FAULT_NONE;

static void lock_state() {
    if (state_mutex != nullptr) {
        (void)xSemaphoreTake(state_mutex, portMAX_DELAY);
    }
}

static void unlock_state() {
    if (state_mutex != nullptr) {
        (void)xSemaphoreGive(state_mutex);
    }
}

bool app_state_init() {
    if (state_mutex == nullptr) {
        state_mutex = xSemaphoreCreateMutex();
    }

    if (state_mutex == nullptr) {
        return false;
    }

    lock_state();
    current_state = SYSTEM_STATE_BOOT;
    current_fault = FAULT_NONE;
    unlock_state();

    return true;
}

void app_state_set(SystemState state) {
    lock_state();
    current_state = state;
    unlock_state();
}

void app_state_apply_event(AppEvent event) {
    lock_state();
    current_state = state_machine_next(current_state, event);
    unlock_state();
}

void app_state_set_fault(FaultCode fault) {
    lock_state();
    current_state = state_machine_next(current_state, APP_EVENT_FAULT_DETECTED);
    current_fault = fault;
    unlock_state();
}

void app_state_clear_fault() {
    lock_state();
    current_state = state_machine_next(current_state, APP_EVENT_RECOVERY_REQUESTED);
    current_fault = FAULT_NONE;
    unlock_state();
}

SystemState app_state_get_state() {
    lock_state();
    const SystemState state = current_state;
    unlock_state();

    return state;
}

FaultCode app_state_get_fault() {
    lock_state();
    const FaultCode fault = current_fault;
    unlock_state();

    return fault;
}

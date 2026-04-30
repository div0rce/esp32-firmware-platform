#include "app_state.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

static SemaphoreHandle_t state_mutex = nullptr;
static SystemState current_state = SystemState::BOOT;
static FaultCode current_fault = FaultCode::NONE;

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
    current_state = SystemState::BOOT;
    current_fault = FaultCode::NONE;
    unlock_state();

    return true;
}

void app_state_set(SystemState state) {
    lock_state();
    current_state = state;
    unlock_state();
}

void app_state_set_fault(FaultCode fault) {
    lock_state();
    current_state = SystemState::FAULT;
    current_fault = fault;
    unlock_state();
}

void app_state_clear_fault() {
    lock_state();
    current_state = SystemState::RECOVERY;
    current_fault = FaultCode::NONE;
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

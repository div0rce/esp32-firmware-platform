#pragma once

#include "fault_state.h"
#include "state_machine.h"

bool app_state_init();
void app_state_set(SystemState state);
void app_state_apply_event(AppEvent event);
void app_state_set_fault(FaultCode fault);
void app_state_clear_fault();
SystemState app_state_get_state();
FaultCode app_state_get_fault();

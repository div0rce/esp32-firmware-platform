#pragma once

#include "fault_state.h"

int adc_raw_to_mv(int raw);
FaultCode fault_for_adc_raw(int raw);

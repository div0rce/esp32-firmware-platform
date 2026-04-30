#pragma once

#include "fault_state.h"

#ifdef __cplusplus
extern "C" {
#endif

int adc_raw_to_mv(int raw);
FaultCode fault_for_adc_raw(int raw);

#ifdef __cplusplus
}
#endif

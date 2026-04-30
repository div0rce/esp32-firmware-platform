#pragma once

#include <stddef.h>

#include "app_config.h"
#include "fault_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADC_RANGE_NORMAL = 0,
    ADC_RANGE_LOW_FAULT,
    ADC_RANGE_HIGH_FAULT
} AdcRangeState;

typedef struct {
    int samples[ADC_FILTER_WINDOW_LENGTH];
    size_t count;
    size_t index;
    long sum;
} AdcMovingAverage;

int adc_raw_to_mv(int raw);
FaultCode fault_for_adc_raw(int raw);
void adc_moving_average_init(AdcMovingAverage* filter);
int adc_moving_average_update(AdcMovingAverage* filter, int raw);
AdcRangeState adc_range_update(AdcRangeState previous_state, int filtered_raw);
FaultCode fault_for_adc_range(AdcRangeState state);

#ifdef __cplusplus
}
#endif

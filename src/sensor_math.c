#include "sensor_math.h"

static int clamp_adc_raw(int raw) {
    if (raw <= ADC_RAW_MIN) {
        return ADC_RAW_MIN;
    }

    if (raw >= ADC_RAW_MAX) {
        return ADC_RAW_MAX;
    }

    return raw;
}

int adc_raw_to_mv(int raw) {
    if (raw <= ADC_RAW_MIN) {
        return 0;
    }

    if (raw >= ADC_RAW_MAX) {
        return ADC_REF_MV;
    }

    const long scaled = (long)raw * ADC_REF_MV;
    return (int)((scaled + (ADC_RAW_MAX / 2)) / ADC_RAW_MAX);
}

FaultCode fault_for_adc_raw(int raw) {
    return fault_for_adc_range(adc_range_update(ADC_RANGE_NORMAL, raw));
}

void adc_moving_average_init(AdcMovingAverage* filter) {
    if (filter == 0) {
        return;
    }

    for (size_t i = 0; i < ADC_FILTER_WINDOW_LENGTH; ++i) {
        filter->samples[i] = 0;
    }

    filter->count = 0;
    filter->index = 0;
    filter->sum = 0;
}

int adc_moving_average_update(AdcMovingAverage* filter, int raw) {
    const int clamped = clamp_adc_raw(raw);

    if (filter == 0) {
        return clamped;
    }

    if (filter->count < ADC_FILTER_WINDOW_LENGTH) {
        filter->samples[filter->index] = clamped;
        filter->sum += clamped;
        filter->count += 1;
    } else {
        filter->sum -= filter->samples[filter->index];
        filter->samples[filter->index] = clamped;
        filter->sum += clamped;
    }

    filter->index = (filter->index + 1U) % ADC_FILTER_WINDOW_LENGTH;

    return (int)(filter->sum / (long)filter->count);
}

AdcRangeState adc_range_update(AdcRangeState previous_state, int filtered_raw) {
    switch (previous_state) {
        case ADC_RANGE_LOW_FAULT:
            return filtered_raw > ADC_LOW_FAULT_CLEAR ? ADC_RANGE_NORMAL : ADC_RANGE_LOW_FAULT;

        case ADC_RANGE_HIGH_FAULT:
            return filtered_raw < ADC_HIGH_FAULT_CLEAR ? ADC_RANGE_NORMAL : ADC_RANGE_HIGH_FAULT;

        case ADC_RANGE_NORMAL:
        default:
            if (filtered_raw < ADC_LOW_FAULT_ENTER) {
                return ADC_RANGE_LOW_FAULT;
            }

            if (filtered_raw > ADC_HIGH_FAULT_ENTER) {
                return ADC_RANGE_HIGH_FAULT;
            }

            return ADC_RANGE_NORMAL;
    }
}

FaultCode fault_for_adc_range(AdcRangeState state) {
    return state == ADC_RANGE_NORMAL ? FAULT_NONE : FAULT_ADC_OUT_OF_RANGE;
}

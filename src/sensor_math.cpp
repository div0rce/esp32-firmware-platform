#include "sensor_math.h"

#include "app_config.h"

int adc_raw_to_mv(int raw) {
    if (raw <= ADC_RAW_MIN) {
        return 0;
    }

    if (raw >= ADC_RAW_MAX) {
        return ADC_REF_MV;
    }

    const long scaled = static_cast<long>(raw) * ADC_REF_MV;
    return static_cast<int>((scaled + (ADC_RAW_MAX / 2)) / ADC_RAW_MAX);
}

FaultCode fault_for_adc_raw(int raw) {
    if (raw < ADC_MIN_VALID || raw > ADC_MAX_VALID) {
        return FaultCode::ADC_OUT_OF_RANGE;
    }

    return FaultCode::NONE;
}

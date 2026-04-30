#include "time_utils.h"

bool elapsed_ms_u32(uint32_t now_ms, uint32_t previous_ms, uint32_t interval_ms) {
    return (uint32_t)(now_ms - previous_ms) >= interval_ms;
}

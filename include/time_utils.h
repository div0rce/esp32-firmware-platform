#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool elapsed_ms_u32(uint32_t now_ms, uint32_t previous_ms, uint32_t interval_ms);

#ifdef __cplusplus
}
#endif

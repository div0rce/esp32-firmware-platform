#pragma once

#include <stddef.h>
#include <stdint.h>

#if defined(USE_ESP32DEV_PINMAP)
#define ADC_PIN ((uint8_t)34)
#define BUTTON_PIN ((uint8_t)25)
#define STATUS_LED_PIN ((uint8_t)2)
#define TRACE_SAMPLE_PIN ((uint8_t)4)
#define TRACE_TELEMETRY_PIN ((uint8_t)5)
#define TRACE_FAULT_PIN ((uint8_t)18)
#else
#define ADC_PIN ((uint8_t)3)
#define BUTTON_PIN ((uint8_t)2)
#define STATUS_LED_PIN ((uint8_t)9)
#define TRACE_SAMPLE_PIN ((uint8_t)4)
#define TRACE_TELEMETRY_PIN ((uint8_t)5)
#define TRACE_FAULT_PIN ((uint8_t)6)
#endif

#define SERIAL_BAUD_RATE ((uint32_t)115200)
#define TELEMETRY_BUFFER_LENGTH ((size_t)256)

#define FIRMWARE_NAME "esp32-firmware-platform"
#define FIRMWARE_VERSION "dev"
#if defined(USE_ESP32DEV_PINMAP)
#define FIRMWARE_TARGET "esp32dev"
#else
#define FIRMWARE_TARGET "seeed_xiao_esp32s3"
#endif

#define ADC_RAW_MIN 0
#define ADC_RAW_MAX 4095
#define ADC_REF_MV 3300
#define ADC_RESOLUTION_BITS 12

#define ADC_MIN_VALID 50
#define ADC_MAX_VALID 4000
#define ADC_FILTER_WINDOW_LENGTH 4
#define ADC_LOW_FAULT_ENTER ADC_MIN_VALID
#define ADC_LOW_FAULT_CLEAR 100
#define ADC_HIGH_FAULT_ENTER ADC_MAX_VALID
#define ADC_HIGH_FAULT_CLEAR 3950

#if defined(ENABLE_STRESS_MODE)
#define SAMPLE_PERIOD_MS ((uint32_t)10)
#define STRESS_TELEMETRY_DELAY_MS ((uint32_t)75)
#define STRESS_FAULT_CYCLE_MS ((uint32_t)4000)
#define STRESS_FAULT_WINDOW_MS ((uint32_t)1500)
#define STRESS_RECOVERY_INJECT_PERIOD_MS ((uint32_t)500)
#define STRESS_OVERFLOW_INJECT_PERIOD_MS ((uint32_t)1000)
#else
#define SAMPLE_PERIOD_MS ((uint32_t)100)
#endif

#define TELEMETRY_PERIOD_MS ((uint32_t)250)
#define BUTTON_DEBOUNCE_MS ((uint32_t)200)
#define APP_WATCHDOG_TIMEOUT_MS ((uint32_t)1000)
#define FAULT_TASK_PERIOD_MS ((uint32_t)50)
#define RUNTIME_HEALTH_PERIOD_MS ((uint32_t)1000)

#define SAMPLE_REQUEST_QUEUE_LENGTH ((size_t)16)
#define SENSOR_QUEUE_LENGTH ((size_t)16)
#define BUTTON_QUEUE_LENGTH ((size_t)8)

#define SENSOR_TASK_STACK ((uint32_t)4096)
#define TELEMETRY_TASK_STACK ((uint32_t)4096)
#define FAULT_TASK_STACK ((uint32_t)2048)
#define BUTTON_TASK_STACK ((uint32_t)2048)

#define SENSOR_TASK_PRIORITY 2
#define TELEMETRY_TASK_PRIORITY 1
#define FAULT_TASK_PRIORITY 3
#define BUTTON_TASK_PRIORITY 3

#define TASK_WATCHDOG_TIMEOUT_SECONDS ((uint32_t)3)

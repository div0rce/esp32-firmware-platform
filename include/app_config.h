#pragma once

#include <cstddef>
#include <cstdint>

constexpr std::uint8_t ADC_PIN = 34;
constexpr std::uint8_t BUTTON_PIN = 25;
constexpr std::uint8_t STATUS_LED_PIN = 2;

constexpr std::uint32_t SERIAL_BAUD_RATE = 115200;
constexpr std::size_t TELEMETRY_BUFFER_LENGTH = 128;

constexpr int ADC_RAW_MIN = 0;
constexpr int ADC_RAW_MAX = 4095;
constexpr int ADC_REF_MV = 3300;
constexpr int ADC_RESOLUTION_BITS = 12;

constexpr int ADC_MIN_VALID = 50;
constexpr int ADC_MAX_VALID = 4000;

constexpr std::uint32_t SAMPLE_PERIOD_MS = 100;
constexpr std::uint32_t TELEMETRY_PERIOD_MS = 250;
constexpr std::uint32_t BUTTON_DEBOUNCE_MS = 200;
constexpr std::uint32_t APP_WATCHDOG_TIMEOUT_MS = 1000;
constexpr std::uint32_t FAULT_TASK_PERIOD_MS = 50;

constexpr std::size_t SAMPLE_REQUEST_QUEUE_LENGTH = 16;
constexpr std::size_t SENSOR_QUEUE_LENGTH = 16;
constexpr std::size_t BUTTON_QUEUE_LENGTH = 8;

constexpr std::uint32_t SENSOR_TASK_STACK = 4096;
constexpr std::uint32_t TELEMETRY_TASK_STACK = 4096;
constexpr std::uint32_t FAULT_TASK_STACK = 2048;
constexpr std::uint32_t BUTTON_TASK_STACK = 2048;

constexpr int SENSOR_TASK_PRIORITY = 2;
constexpr int TELEMETRY_TASK_PRIORITY = 1;
constexpr int FAULT_TASK_PRIORITY = 3;
constexpr int BUTTON_TASK_PRIORITY = 3;

constexpr std::uint32_t TASK_WATCHDOG_TIMEOUT_SECONDS = 3;

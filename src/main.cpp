#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "app_config.h"
#include "app_state.h"
#include "button.h"
#include "fault_state.h"
#include "manufacturing_self_test.h"
#include "runtime_health.h"
#include "sampling_timer.h"
#include "sensor.h"
#include "telemetry.h"
#include "time_utils.h"
#include "watchdog.h"

static QueueHandle_t sample_request_queue = nullptr;
static QueueHandle_t sensor_sample_queue = nullptr;
static QueueHandle_t button_event_queue = nullptr;

static TaskHandle_t sensor_task_handle = nullptr;
static TaskHandle_t telemetry_task_handle = nullptr;
static TaskHandle_t button_task_handle = nullptr;
static TaskHandle_t fault_task_handle = nullptr;

static portMUX_TYPE sample_timestamp_mux = portMUX_INITIALIZER_UNLOCKED;
static std::uint32_t last_sample_timestamp_ms = 0;

static void record_sample_timestamp(std::uint32_t timestamp_ms) {
    portENTER_CRITICAL(&sample_timestamp_mux);
    last_sample_timestamp_ms = timestamp_ms;
    portEXIT_CRITICAL(&sample_timestamp_mux);
}

static std::uint32_t get_last_sample_timestamp() {
    portENTER_CRITICAL(&sample_timestamp_mux);
    const std::uint32_t timestamp_ms = last_sample_timestamp_ms;
    portEXIT_CRITICAL(&sample_timestamp_mux);

    return timestamp_ms;
}

static void enter_fault(FaultCode fault) {
    app_state_set_fault(fault);
}

#ifdef ENABLE_TRACE_PINS
static void trace_pins_init() {
    pinMode(TRACE_SAMPLE_PIN, OUTPUT);
    pinMode(TRACE_TELEMETRY_PIN, OUTPUT);
    pinMode(TRACE_FAULT_PIN, OUTPUT);

    digitalWrite(TRACE_SAMPLE_PIN, LOW);
    digitalWrite(TRACE_TELEMETRY_PIN, LOW);
    digitalWrite(TRACE_FAULT_PIN, LOW);
}

static void trace_pulse_begin(uint8_t pin) {
    digitalWrite(pin, HIGH);
}

static void trace_pulse_end(uint8_t pin) {
    digitalWrite(pin, LOW);
}
#else
static void trace_pins_init() {}
static void trace_pulse_begin(uint8_t) {}
static void trace_pulse_end(uint8_t) {}
#endif

static void sensor_task(void* parameter) {
    (void)parameter;
    (void)watchdog_register_current_task();

    SampleRequest request;

    while (true) {
        if (xQueueReceive(sample_request_queue, &request, portMAX_DELAY) == pdPASS) {
            app_state_apply_event(APP_EVENT_SAMPLE_STARTED);

            trace_pulse_begin(TRACE_SAMPLE_PIN);
            const SensorSample sample = read_sensor_sample(request);
            trace_pulse_end(TRACE_SAMPLE_PIN);
            record_sample_timestamp(sample.timestamp_ms);
            runtime_health_record_sensor_sample();

            if (!sample_is_valid(sample)) {
                enter_fault(sample.fault);
            }

            if (xQueueSend(sensor_sample_queue, &sample, 0) != pdPASS) {
                enter_fault(FAULT_SENSOR_QUEUE_SEND_FAILED);
            }
        }

        watchdog_reset_current_task();
    }
}

static void telemetry_task(void* parameter) {
    (void)parameter;
    (void)watchdog_register_current_task();

    SensorSample sample;
    std::uint32_t last_health_ms = 0;

    while (true) {
        const BaseType_t received = xQueueReceive(
            sensor_sample_queue,
            &sample,
            pdMS_TO_TICKS(TELEMETRY_PERIOD_MS)
        );

        if (received == pdPASS) {
            const bool valid_sample = sample_is_valid(sample);
            const bool active_fault = app_state_get_fault() != FAULT_NONE;
            const SystemState state = (valid_sample && !active_fault)
                ? SYSTEM_STATE_TRANSMIT
                : SYSTEM_STATE_FAULT;

            if (state == SYSTEM_STATE_TRANSMIT) {
                app_state_apply_event(APP_EVENT_TRANSMIT_STARTED);
            }

            trace_pulse_begin(TRACE_TELEMETRY_PIN);
            telemetry_print_sample(state, sample);
            trace_pulse_end(TRACE_TELEMETRY_PIN);
            runtime_health_record_telemetry_packet();

            if (valid_sample && app_state_get_fault() == FAULT_NONE) {
                app_state_apply_event(APP_EVENT_TRANSMIT_COMPLETE);
            }
        }

        const std::uint32_t now_ms = millis();
        if (last_health_ms == 0U || elapsed_ms_u32(now_ms, last_health_ms, RUNTIME_HEALTH_PERIOD_MS)) {
            RuntimeHealthInputs inputs;
            inputs.timestamp_ms = now_ms;
            inputs.sensor_task = sensor_task_handle;
            inputs.telemetry_task = telemetry_task_handle;
            inputs.button_task = button_task_handle;
            inputs.fault_task = fault_task_handle;
            inputs.sample_request_queue = sample_request_queue;
            inputs.sensor_sample_queue = sensor_sample_queue;
            inputs.button_event_queue = button_event_queue;

            telemetry_print_runtime_health(runtime_health_capture(inputs));
            runtime_health_record_telemetry_packet();
            last_health_ms = now_ms;
        }

        watchdog_reset_current_task();
    }
}

static void button_task(void* parameter) {
    (void)parameter;
    (void)watchdog_register_current_task();

    ButtonEvent event;
    std::uint32_t last_accepted_ms = 0;
    const TickType_t button_event_timeout = pdMS_TO_TICKS(100);

    while (true) {
        if (button_receive_event(&event, button_event_timeout)) {
            runtime_health_record_button_event();

            if (last_accepted_ms == 0U || elapsed_ms_u32(event.timestamp_ms, last_accepted_ms, BUTTON_DEBOUNCE_MS)) {
                last_accepted_ms = event.timestamp_ms;

                if (app_state_get_state() == SYSTEM_STATE_FAULT) {
                    app_state_clear_fault();
                    app_state_apply_event(APP_EVENT_RECOVERY_COMPLETE);
                }
            }
        }

        watchdog_reset_current_task();
    }
}

static void fault_task(void* parameter) {
    (void)parameter;
    (void)watchdog_register_current_task();

    while (true) {
        trace_pulse_begin(TRACE_FAULT_PIN);
        runtime_health_record_fault_check();

        if (button_consume_overflow()) {
            enter_fault(FAULT_BUTTON_QUEUE_SEND_FAILED);
        }

        if (sampling_timer_consume_overflow()) {
            enter_fault(FAULT_SAMPLE_TIMER_FAILED);
        }

        const std::uint32_t now_ms = millis();
        const std::uint32_t last_sample_ms = get_last_sample_timestamp();

        if (last_sample_ms != 0U && elapsed_ms_u32(now_ms, last_sample_ms, APP_WATCHDOG_TIMEOUT_MS)) {
            enter_fault(FAULT_WATCHDOG_TIMEOUT);
        }

        const bool fault_active = app_state_get_state() == SYSTEM_STATE_FAULT;
        digitalWrite(STATUS_LED_PIN, fault_active ? HIGH : LOW);

        trace_pulse_end(TRACE_FAULT_PIN);

        watchdog_reset_current_task();
        vTaskDelay(pdMS_TO_TICKS(FAULT_TASK_PERIOD_MS));
    }
}

static bool create_queues() {
    sample_request_queue = xQueueCreate(SAMPLE_REQUEST_QUEUE_LENGTH, sizeof(SampleRequest));
    sensor_sample_queue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorSample));
    button_event_queue = xQueueCreate(BUTTON_QUEUE_LENGTH, sizeof(ButtonEvent));

    return sample_request_queue != nullptr
        && sensor_sample_queue != nullptr
        && button_event_queue != nullptr;
}

static bool create_tasks() {
    return xTaskCreate(sensor_task, "sensor_task", SENSOR_TASK_STACK, nullptr, SENSOR_TASK_PRIORITY, &sensor_task_handle) == pdPASS
        && xTaskCreate(telemetry_task, "telemetry_task", TELEMETRY_TASK_STACK, nullptr, TELEMETRY_TASK_PRIORITY, &telemetry_task_handle) == pdPASS
        && xTaskCreate(button_task, "button_task", BUTTON_TASK_STACK, nullptr, BUTTON_TASK_PRIORITY, &button_task_handle) == pdPASS
        && xTaskCreate(fault_task, "fault_task", FAULT_TASK_STACK, nullptr, FAULT_TASK_PRIORITY, &fault_task_handle) == pdPASS;
}

void setup() {
    telemetry_init();

    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);
    trace_pins_init();

    if (!app_state_init() || !watchdog_init() || !create_queues()) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        while (true) {
            delay(1000);
        }
    }

    sensor_init();
    const ManufacturingSelfTestResult self_test = manufacturing_self_test_run();
    telemetry_print_self_test_result(millis(), self_test);

    if (!self_test.passed) {
        enter_fault(FAULT_MANUFACTURING_SELF_TEST_FAILED);
        digitalWrite(STATUS_LED_PIN, HIGH);
    }

    button_init(button_event_queue);

    app_state_apply_event(APP_EVENT_BOOT_COMPLETE);

    if (!create_tasks() || !sampling_timer_start(sample_request_queue)) {
        enter_fault(FAULT_SAMPLE_TIMER_FAILED);
        digitalWrite(STATUS_LED_PIN, HIGH);
    }
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}

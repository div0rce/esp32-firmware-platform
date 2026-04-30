#pragma once

bool watchdog_init();
bool watchdog_register_current_task();
void watchdog_reset_current_task();

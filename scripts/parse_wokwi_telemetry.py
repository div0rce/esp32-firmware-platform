#!/usr/bin/env python3
import re
import sys
from pathlib import Path


ADC_RE = re.compile(r"\badc_raw=(\d+)\b")
FAULT_RE = re.compile(r"\bfault=([A-Z0-9_]+)\b")


def empty_summary():
    return {
        "serial_bytes": 0,
        "task_wdt_count": 0,
        "adc_fault_count": 0,
        "normal_count": 0,
        "min_adc_raw": 0,
        "max_adc_raw": 0,
        "first_fault_line": "NONE",
    }


def print_summary(summary):
    for key in (
        "serial_bytes",
        "task_wdt_count",
        "adc_fault_count",
        "normal_count",
        "min_adc_raw",
        "max_adc_raw",
        "first_fault_line",
    ):
        print(f"{key}={summary[key]}")


def fail(summary, errors):
    print_summary(summary)
    for error in errors:
        print(f"error={error}", file=sys.stderr)
    return 1


def main():
    summary = empty_summary()

    if len(sys.argv) != 2:
        return fail(summary, ["usage: parse_wokwi_telemetry.py <serial-log-path>"])

    log_path = Path(sys.argv[1])
    if not log_path.is_file():
        return fail(summary, [f"serial log file does not exist: {log_path}"])

    data = log_path.read_text(errors="replace")
    summary["serial_bytes"] = len(data.encode())

    if summary["serial_bytes"] == 0:
        return fail(summary, ["serial log file is empty"])

    adc_values = []
    telemetry_lines = 0
    errors = []

    for line in data.splitlines():
        stripped = line.strip()
        lower = stripped.lower()

        if "task_wdt" in lower:
            summary["task_wdt_count"] += 1

        if "ADC_OUT_OF_RANGE" in stripped:
            summary["adc_fault_count"] += 1

        adc_match = ADC_RE.search(stripped)
        fault_match = FAULT_RE.search(stripped)

        if adc_match or fault_match:
            telemetry_lines += 1

        if adc_match:
            adc_values.append(int(adc_match.group(1)))

        if fault_match:
            fault = fault_match.group(1)
            if fault == "NONE":
                summary["normal_count"] += 1
            elif summary["first_fault_line"] == "NONE":
                summary["first_fault_line"] = stripped

    if adc_values:
        summary["min_adc_raw"] = min(adc_values)
        summary["max_adc_raw"] = max(adc_values)

    if telemetry_lines == 0:
        errors.append("no parseable telemetry lines found")

    if not adc_values:
        errors.append("no adc_raw values found")

    if summary["task_wdt_count"] > 0:
        errors.append("task watchdog reset detected")

    if summary["normal_count"] == 0:
        errors.append("no fault=NONE telemetry found")

    if summary["adc_fault_count"] > 0 and summary["normal_count"] == 0:
        errors.append("persistent ADC_OUT_OF_RANGE fault detected")

    if errors:
        return fail(summary, errors)

    print_summary(summary)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

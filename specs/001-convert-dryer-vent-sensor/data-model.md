# Data Model: Convert Dryer Vent Sensor to External Component

**Date**: 2025-12-28
**Feature**: specs/001-convert-dryer-vent-sensor/spec.md

## Entities

### Pulse Packet
Represents detected signal patterns from the dryer vent hardware.

**Fields**:
- `pulse_count`: Integer (1, 3, or 5) - Number of pulses in the packet
- `pulse_length`: Integer - Duration of pulse sequence in timer ticks
- `timestamp`: Integer - Milliseconds since boot when packet detected

**Relationships**:
- Triggers updates to Sensor State entities
- Increments Counter Values based on pulse_count

**Validation Rules**:
- pulse_count MUST be 1, 3, or 5
- pulse_length MUST be > 0
- timestamp MUST be valid millis() value

### Sensor State
Boolean/binary values representing current system conditions.

**Fields**:
- `sensor_type`: String enum ("clog", "overheat", "selftest") - Type of condition monitored
- `current_value`: Boolean - Current state (true = condition detected/failure)
- `last_updated`: Integer - Milliseconds since boot of last state change

**Relationships**:
- Updated by Pulse Packet detection (clog/overheat) or self-test results (selftest)
- Published to ESPHome sensor entities

**Validation Rules**:
- sensor_type MUST be one of defined enum values
- current_value MUST be boolean
- last_updated MUST be valid millis() value

**State Transitions**:
- clog: false → true when 3-pulse packet detected
- overheat: false → true when 5-pulse packet detected
- selftest: false → true when test fails, resets to false on success

### Pin Configuration
GPIO pin assignments for hardware interface.

**Fields**:
- `pulse_pin`: Integer (default 2) - GPIO pin for pulse counter input
- `test_pin`: Integer (default 15) - GPIO pin for self-test output

**Relationships**:
- Used by component setup to configure GPIO and pulse counter
- Must be valid ESP32 GPIO pins not used by other components

**Validation Rules**:
- pulse_pin MUST be valid ESP32 GPIO pin (0-39)
- test_pin MUST be valid ESP32 GPIO pin (0-39)
- pulse_pin and test_pin MUST be different
- Pins MUST not conflict with other ESPHome components
# Feature Specification: Convert Dryer Vent Sensor to External Component

**Feature Branch**: `001-convert-dryer-vent-sensor`  
**Created**: 2025-12-28  
**Status**: Draft  
**Input**: User description: "we are converting dryer_vent_sensor.h into an external component. our new component should accomplish creating the same sensors and monitoring the same hardware device"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Monitor Dryer Vent Clog Status (Priority: P1)

As an ESPHome user, I want to monitor if my dryer vent is clogged so I can clean it when needed and prevent inefficient drying.

**Why this priority**: Clog detection is critical for safety and efficiency, preventing fire hazards and energy waste.

**Independent Test**: Can be fully tested by simulating 3-pulse packet pattern and verifying clog sensor publishes true, delivering value for vent maintenance.

**Acceptance Scenarios**:

1. **Given** dryer vent sensor is configured and running, **When** a 3-pulse packet is detected on the pulse counter, **Then** the clog sensor publishes true
2. **Given** no clog condition detected, **When** normal operation continues, **Then** the clog sensor remains false

---

### User Story 2 - Monitor Dryer Vent Overheat Status (Priority: P2)

As an ESPHome user, I want to monitor if my dryer vent is overheating so I can prevent fire hazards.

**Why this priority**: Overheat detection is essential for safety, allowing timely intervention.

**Independent Test**: Can be fully tested by simulating 5-pulse packet pattern and verifying overheat sensor publishes true, delivering value for fire prevention.

**Acceptance Scenarios**:

1. **Given** dryer vent sensor is configured and running, **When** a 5-pulse packet is detected on the pulse counter, **Then** the overheat sensor publishes true

---

### User Story 3 - Receive Self-Test Notifications (Priority: P3)

As an ESPHome user, I want to receive notifications when the sensor performs self-tests to ensure it's working properly.

**Why this priority**: Ensures reliability of the monitoring system through automated health checks.

**Independent Test**: Can be fully tested by triggering self-test mechanism and verifying selftest sensor updates appropriately, delivering value for system reliability assurance.

**Acceptance Scenarios**:

1. **Given** sensor is configured and self-test is triggered, **When** test response is detected within timeout, **Then** selftest sensor publishes false (success)
2. **Given** sensor is configured and self-test is triggered, **When** no test response within timeout, **Then** selftest sensor publishes true (failure)

### Edge Cases

- What happens when multiple pulse packets overlap in timing?
- How does system handle invalid or unexpected pulse counts (not 1, 3, or 5)?
- What if the hardware pulse counter fails or GPIO pin is misconfigured?
- How does system behave during power interruptions or resets?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Component MUST create overheat sensor that publishes boolean state
- **FR-002**: Component MUST create clog sensor that publishes boolean state  
- **FR-003**: Component MUST create selftest sensor that publishes boolean state (true=failure)
- **FR-004**: Component MUST create packet counter sensors for short_packet, short_start_packet, long_start_packet, short_clog_packet, long_clog_packet, short_overheat_packet, long_overheat_packet, unknown_packet_sensor, selftest_counter
- **FR-005**: Component MUST monitor pulse counter on GPIO pin 2 for incoming pulses
- **FR-006**: Component MUST perform automated self-tests every 24 hours by triggering GPIO pin 15
- **FR-007**: Component MUST detect 1-pulse packets for start/test conditions
- **FR-008**: Component MUST detect 3-pulse packets for clog conditions
- **FR-009**: Component MUST detect 5-pulse packets for overheat conditions
- **FR-010**: Component MUST update all sensors every 15 seconds during polling
- **FR-011**: Component MUST allow configuration of GPIO pin for pulse input (default: 2)
- **FR-012**: Component MUST allow configuration of GPIO pin for test output (default: 15)

### Key Entities *(include if feature involves data)*

- **Pulse Packet**: Represents detected signal patterns with attributes: pulse_count (1, 3, or 5), pulse_length, timestamp
- **Sensor State**: Boolean/binary values for system conditions with attributes: sensor_type (clog/overheat/test), current_value, last_updated
- **Counter Values**: Integer counts for various packet types with attributes: counter_type, current_count, increment_events
- **Pin Configuration**: GPIO pin assignments with attributes: pulse_pin (default 2), test_pin (default 15)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Component detects and reports clog condition within 15 seconds of receiving 3-pulse pattern
- **SC-002**: Component detects and reports overheat condition within 15 seconds of receiving 5-pulse pattern
- **SC-003**: Self-test mechanism completes evaluation within 3 polling cycles (45 seconds) or marks test as failed
- **SC-004**: All sensors publish state updates every 15 seconds during normal operation
- **SC-005**: Component builds cleanly without warnings in ESPHome compilation environment

## Constitution Compliance *(mandatory)*

- **CC-001**: Feature MUST follow all ESPHome guidelines and best practices
- **CC-002**: Feature MUST implement Hardware Abstraction where applicable
- **CC-003**: Feature MUST follow Test-First development (tests defined before implementation)
- **CC-004**: Feature MUST include comprehensive Documentation
- **CC-005**: Feature MUST adhere to Versioning & Simplicity principles
- **CC-006**: Feature MUST build cleanly without warnings

# Research: Convert Dryer Vent Sensor to External Component

**Date**: 2025-12-28
**Feature**: specs/001-convert-dryer-vent-sensor/spec.md

## Findings

### ESPHome External Component Structure

**Decision**: Use standard ESPHome external component structure with `components/` directory containing Python files and optional C++ headers.

**Rationale**: Follows ESPHome conventions for external components, allows clean separation from core ESPHome, enables independent versioning and distribution.

**Alternatives Considered**:
- Inline component in user YAML: Rejected because requires copying code, hard to maintain updates
- Custom integration via custom_components: Rejected because less reusable than external component

### ESP32 Pulse Counter API

**Decision**: Use ESP-IDF pcnt (pulse counter) peripheral with pcnt_unit_config, pcnt_set_filter_value, pcnt_counter_resume for hardware pulse counting.

**Rationale**: Direct hardware access provides accurate pulse detection at 50Hz sampling rate, essential for real-time vent monitoring.

**Alternatives Considered**:
- Software pulse counting via GPIO interrupts: Rejected due to higher CPU usage and potential timing issues
- External ADC sampling: Rejected as overkill for digital pulse signals

### Component Conversion Approach

**Decision**: Extract class logic from dryer_vent_sensor.h into external component sensor.py, maintain same sensor creation and update logic.

**Rationale**: Preserves existing functionality while making it reusable as external component, follows ESPHome component patterns.

**Alternatives Considered**:
- Complete rewrite: Rejected because increases risk of introducing bugs
- Wrapper approach: Rejected as adds unnecessary complexity

### Testing Strategy

**Decision**: Use ESPHome test framework with mocked hardware for unit tests, ESP32 simulator for integration tests.

**Rationale**: Allows testing component logic without physical hardware, ensures clean builds and reliable CI/CD.

**Alternatives Considered**:
- Manual testing only: Rejected due to lack of automation and regression prevention
- Full hardware testing: Rejected as requires physical setup, slows development

### Documentation Standards

**Decision**: Include README.md with installation, configuration examples, and troubleshooting; use ESPHome documentation format.

**Rationale**: Follows ESPHome community standards, makes component accessible to users.

**Alternatives Considered**:
- Minimal documentation: Rejected because external components need clear usage guides
- Wiki-only: Rejected as less discoverable than repo README
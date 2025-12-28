<!--
Sync Impact Report:
Version change: 1.3.0 → 1.4.0
List of modified principles: VIII. Truthful Validation (added)
Added sections: None
Removed sections: None
Templates requiring updates: plan-template.md (✅ updated)
Follow-up TODOs: None
-->

# ESPHome Laundry External Component Constitution
<!-- Example: Spec Constitution, TaskFlow Constitution, etc. -->

## Core Principles

### I. ESPHome Compatibility
Every component must follow all ESPHome guidelines and best practices; Components must integrate seamlessly with the ESPHome framework; Components must follow ESPHome's YAML configuration patterns; Clear hardware requirements documented

### II. Hardware Abstraction
Abstract sensor/actuator specifics for portability; Support multiple hardware variants; Document pin mappings and requirements

### III. Test-First (NON-NEGOTIABLE)
TDD mandatory: Tests written → User approved → Tests fail → Then implement; Red-Green-Refactor cycle strictly enforced; Unit tests for core logic, integration tests for hardware interaction; All builds must compile cleanly without warnings

### IV. Documentation
Every component includes comprehensive user documentation; API examples provided; Configuration templates included; Troubleshooting guides for common issues

### V. Versioning & Simplicity
Semantic versioning (MAJOR.MINOR.PATCH) for all releases; Breaking changes require major version bump; Start simple, avoid over-engineering; YAGNI principles applied

### VI. Validation Repeatability
All code validation procedures must be scripted for team repeatability; Validation scripts stored in repository under scripts/ directory; Scripts must be executable by any team member without manual setup; Automated validation preferred over manual checks

### VII. Host System Isolation
Minimize host system dependencies and avoid modifying system state; Use virtual environments for Python tooling; Prefer lightweight containers (Docker) only when virtual environments insufficient; Never require system-wide package installations; Development environment must be reproducible across different host systems

### VIII. Truthful Validation
Validation scripts must accurately represent what they validate; Never claim success for incomplete or skipped validations; Clearly communicate validation scope and limitations; Prefer failing fast over deceptive success messages

## Additional Constraints
Technology stack: C++ for components, YAML for configuration; Target platforms: ESP32, ESP8266; Memory constraints: <50KB RAM usage per component; Power efficiency: Low-power modes supported where applicable

## Development Workflow
Code review required for all changes; Testing gates: Unit tests pass, integration tests pass, builds clean without warnings; Deployment: Via ESPHome releases; CI/CD: Automated builds and tests on ESP32 simulator

## Governance
Constitution supersedes all other practices; Amendments require pull request review and approval; Changes must include migration plan for existing users; Compliance verified in code reviews

**Version**: 1.4.0 | **Ratified**: 2025-12-28 | **Last Amended**: 2025-12-28
<!-- Example: Version: 2.1.1 | Ratified: 2025-06-13 | Last Amended: 2025-07-16 -->

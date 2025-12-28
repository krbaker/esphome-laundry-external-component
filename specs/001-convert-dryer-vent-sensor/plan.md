# Implementation Plan: Convert Dryer Vent Sensor to External Component

**Branch**: `001-convert-dryer-vent-sensor` | **Date**: 2025-12-28 | **Spec**: specs/001-convert-dryer-vent-sensor/spec.md
**Input**: Feature specification from `/specs/[001-convert-dryer-vent-sensor]/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Convert dryer_vent_sensor.h into an ESPHome external component that creates the same sensors and monitors the same hardware device. Technical approach uses ESPHome external component structure with Python configuration and C++ sensor implementation, leveraging ESP32 pulse counter for hardware monitoring.

## Technical Context

<!--
  ACTION REQUIRED: Replace the content in this section with the technical details
  for the project. The structure here is presented in advisory capacity to guide
  the iteration process.
-->

## Technical Context

**Language/Version**: C++ (ESPHome component), Python (configuration)  
**Primary Dependencies**: ESPHome framework, ESP-IDF  
**Storage**: N/A (sensor publishing to ESPHome)  
**Testing**: ESPHome test framework with ESP32 simulator  
**Target Platform**: ESP32  
**Project Type**: ESPHome external component (single library)  
**Performance Goals**: Detect clog/overheat within 15 seconds, poll sensors every 15 seconds  
**Constraints**: <50KB RAM usage, clean builds without warnings, configurable GPIO pins (defaults 2 input, 15 output)  
**Scale/Scope**: Single component exposing 12 sensors for vent monitoring

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Verify all ESPHome guidelines and best practices are followed for the component
- Hardware abstraction layer designed for portability
- Test-first approach confirmed (tests planned before implementation)
- Documentation plan includes user guides and examples
- Versioning strategy follows semantic versioning principles
- Build process ensures clean compilation without warnings
- Validation scripts created for all code validation procedures
- Host system isolation maintained (virtual environments preferred)
- Validation scripts provide truthful feedback about what they validate

## Validation Strategy

**Constitution Principle VI**: All validation procedures must be scripted for team repeatability.

### Validation Scripts

- **validate-syntax.sh**: Checks Python syntax in components/ and tests/
- **validate-build.sh**: Validates ESPHome component compilation (ESP32 target)
- **validate-tests.sh**: Runs unit tests and reports coverage

### Execution Requirements

- Scripts stored in `scripts/` directory at repository root
- Executable by any team member without manual setup
- Return non-zero exit codes on validation failures
- Include help text and usage examples

### Integration

- Scripts integrated into development workflow
- Automated execution in CI/CD pipeline
- Manual execution for local validation before commits

## Development Environment Strategy

**Constitution Principle VII**: Host system isolation with minimal dependencies.

### Tooling Approach

- **Python Development**: Virtual environment (venv) for all Python tooling
- **Testing**: pytest within virtual environment
- **Build Validation**: Local scripts without system modifications
- **CI/CD**: Docker containers for full ESPHome builds when needed

### Environment Setup

- Repository includes `requirements-dev.txt` for development dependencies
- Virtual environment activation documented in quickstart
- No system-wide package installations required
- Development environment reproducible across Windows/Linux/macOS

### Container Usage

- Docker preferred only for ESPHome full builds (not local development)
- Lightweight container images for CI/CD
- Local development uses native tools in virtual environments

## Build Validation Strategy

**Constitution Principle VIII**: Truthful validation requires actual build testing.

### ESPHome Build Environment Setup

- Use ESPHome Docker container for isolated, reproducible builds
- Pull official ESPHome Docker image for consistent toolchain
- Mount component source code into container for testing

### Full Component Build Process

1. **Test Configuration Creation**: Generate minimal ESPHome YAML using the component
2. **Docker Build Execution**: Run `esphome compile` in container against test config
3. **Error Detection**: Capture and report any compilation failures
4. **Success Validation**: Confirm clean build without warnings

### Build Validation Script Enhancement

- Extend `validate-build.sh` to include optional full ESPHome build
- Use Docker when available, fall back to structure validation
- Clearly communicate build vs structure validation scope

### CI/CD Integration

- Automated full builds in CI/CD pipeline using ESPHome containers
- Build artifacts stored for deployment validation
- Failed builds block releases per testing gates

## Project Structure

### Documentation (this feature)

```text
specs/001-convert-dryer-vent-sensor/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
components/
dryer_vent_sensor/
├── __init__.py          # Component registration
├── sensor.py            # Python configuration and sensor definitions
└── sensor.h             # C++ sensor implementation (converted from dryer_vent_sensor.h)

tests/
├── test_sensor.py       # Unit and integration tests
└── conftest.py          # Test fixtures and mocks

scripts/
├── validate-build.sh    # Build validation script
├── validate-syntax.sh   # Syntax checking script
└── validate-tests.sh    # Test execution script

requirements-dev.txt     # Development dependencies for virtual environment
```

**Structure Decision**: ESPHome external component structure with components/ directory containing Python configuration files and optional C++ headers, plus tests/ directory for automated testing. This follows ESPHome best practices for external components and enables clean separation of concerns.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No constitution violations identified - all guidelines followed.

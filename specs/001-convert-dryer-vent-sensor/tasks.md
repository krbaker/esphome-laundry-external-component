---

description: "Task list for converting dryer vent sensor to ESPHome external component"
---

# Tasks: Convert Dryer Vent Sensor to External Component

**Input**: Design documents from `/specs/001-convert-dryer-vent-sensor/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: The examples below include test tasks. Tests are OPTIONAL - only include them if explicitly requested in the feature specification.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: `src/`, `tests/` at repository root
- **Web app**: `backend/src/`, `frontend/src/`
- **Mobile**: `api/src/`, `ios/src/` or `android/src/`
- Paths shown below assume single project - adjust based on plan.md structure

<!-- 
  ============================================================================
  IMPORTANT: The tasks below are SAMPLE TASKS for illustration purposes only.
  
  The /speckit.tasks command MUST replace these with actual tasks based on:
  - User stories from spec.md (with their priorities P1, P2, P3...)
  - Feature requirements from plan.md
  - Entities from data-model.md
  - Endpoints from contracts/
  
  Tasks MUST be organized by user story so each story can be:
  - Implemented independently
  - Tested independently
  - Delivered as an MVP increment
  
  DO NOT keep these sample tasks in the generated tasks.md file.
  ============================================================================
-->

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [x] T001 Create project structure per implementation plan
- [x] T002 Initialize ESPHome external component with dependencies
- [x] T003 [P] Configure linting and formatting tools

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

Examples of foundational tasks (adjust based on your project):

- [x] T004 Setup ESPHome component base structure
- [x] T005 [P] Implement pin configuration handling
- [x] T006 [P] Setup pulse counter infrastructure
- [x] T007 Create base sensor entity classes

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Monitor Dryer Vent Clog Status (Priority: P1) 🎯 MVP

**Goal**: Enable users to monitor dryer vent clog status for maintenance

**Independent Test**: Can be fully tested by simulating 3-pulse pattern and verifying clog sensor publishes true

### Tests for User Story 1 (OPTIONAL - only if tests requested) ⚠️

> **NOTE: Write these tests FIRST, ensure they FAIL before implementation**

- [ ] T008 [P] [US1] Contract test for clog sensor in tests/contract/test_clog.py
- [ ] T009 [P] [US1] Integration test for clog detection in tests/integration/test_clog.py

### Implementation for User Story 1

- [x] T010 [P] [US1] Create clog sensor entity in components/dryer_vent_sensor/sensor.py
- [x] T011 [US1] Implement 3-pulse packet detection logic in components/dryer_vent_sensor/sensor.h
- [x] T012 [US1] Add clog sensor publishing in components/dryer_vent_sensor/sensor.h
- [x] T013 [US1] Update sensor polling to detect clog conditions

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Monitor Dryer Vent Overheat Status (Priority: P2)

**Goal**: Enable users to monitor dryer vent overheat status for safety

**Independent Test**: Can be fully tested by simulating 5-pulse pattern and verifying overheat sensor publishes true

### Tests for User Story 2 (OPTIONAL - only if tests requested) ⚠️

- [ ] T014 [P] [US2] Contract test for overheat sensor in tests/contract/test_overheat.py
- [ ] T015 [P] [US2] Integration test for overheat detection in tests/integration/test_overheat.py

### Implementation for User Story 2

- [x] T016 [P] [US2] Create overheat sensor entity in components/dryer_vent_sensor/sensor.py
- [x] T017 [US2] Implement 5-pulse packet detection logic in components/dryer_vent_sensor/sensor.h
- [x] T018 [US2] Add overheat sensor publishing in components/dryer_vent_sensor/sensor.h
- [x] T019 [US2] Update sensor polling to detect overheat conditions

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Receive Self-Test Notifications (Priority: P3)

**Goal**: Enable users to receive self-test notifications for sensor reliability

**Independent Test**: Can be fully tested by triggering self-test and verifying selftest sensor updates

### Tests for User Story 3 (OPTIONAL - only if tests requested) ⚠️

- [ ] T020 [P] [US3] Contract test for selftest sensor in tests/contract/test_selftest.py
- [ ] T021 [P] [US3] Integration test for self-test mechanism in tests/integration/test_selftest.py

### Implementation for User Story 3

- [x] T022 [P] [US3] Create selftest sensor entity in components/dryer_vent_sensor/sensor.py
- [x] T023 [US3] Implement self-test trigger logic in components/dryer_vent_sensor/sensor.h
- [x] T024 [US3] Add selftest sensor publishing in components/dryer_vent_sensor/sensor.h
- [x] T025 [US3] Update sensor polling to handle self-test timing and results

**Checkpoint**: All user stories should now be independently functional

---

## Phase N: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [x] T026 [P] Add remaining sensor entities (packet counters) in components/dryer_vent_sensor/sensor.py
- [x] T027 Implement packet counter logic in components/dryer_vent_sensor/sensor.h
- [x] T028 Add comprehensive documentation in README.md
- [x] T029 [P] Run build validation to ensure clean compilation
- [x] T030 Create example configuration files
- [x] T031 Update quickstart.md with complete examples

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - No dependencies on other stories

### Within Each User Story

- Tests (if included) MUST be written and FAIL before implementation
- Sensor entities before detection logic
- Detection logic before publishing
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Once Foundational phase completes, all user stories can start in parallel (if team capacity allows)
- All tests for a user story marked [P] can run in parallel
- Sensor creation within a story marked [P] can run in parallel
- Different user stories can be worked on in parallel by different team members

---

## Parallel Example: User Story 1

```bash
# Launch all tests for User Story 1 together (if tests requested):
Task: "Contract test for clog sensor in tests/contract/test_clog.py"
Task: "Integration test for clog detection in tests/integration/test_clog.py"

# Launch all sensor creation for User Story 1 together:
Task: "Create clog sensor entity in components/dryer_vent_sensor/sensor.py"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently
5. Deploy/demo if ready

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 2 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1
   - Developer B: User Story 2
   - Developer C: User Story 3
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Verify tests fail before implementing
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence

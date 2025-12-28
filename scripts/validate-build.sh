#!/bin/bash

# validate-build.sh - Validate ESPHome component structure and basic compatibility
# Constitution Principle VI: Validation scripts for team repeatability
# Constitution Principle VII: Host system isolation via virtual environments
# Constitution Principle VIII: Truthful validation - clearly communicate what is validated

set -e

echo "🔨 Validating ESPHome component structure..."

# Check for virtual environment (Principle VII)
if [ -z "$VIRTUAL_ENV" ]; then
    echo "⚠️  No virtual environment detected. Please activate virtual environment:"
    echo "    python3 -m venv venv && source venv/bin/activate && pip install -r requirements-dev.txt"
    echo "    Then re-run this script."
    exit 1
fi

PYTHON_CMD="$VIRTUAL_ENV/bin/python"

# Check if example YAML files are valid YAML
echo "Checking example configurations..."
if command -v yamllint >/dev/null 2>&1; then
    find examples/ -name "*.yaml" -o -name "*.yml" | xargs yamllint --no-warnings
else
    echo "⚠️  yamllint not found, skipping YAML validation (install with: pip install yamllint)"
fi

# Check if Python component structure is valid
echo "Testing component structure..."
"$PYTHON_CMD" -c "
import sys
import os
sys.path.insert(0, 'components')

# Check if required files exist
required_files = [
    'components/dryer_vent_sensor/__init__.py',
    'components/dryer_vent_sensor/sensor.h'
]
for file in required_files:
    if not os.path.exists(file):
        print(f'❌ Required file missing: {file}')
        sys.exit(1)

print('✅ Component structure validation passed')
"

# Check if Docker is available for full ESPHome builds
echo "Checking for full ESPHome build capability..."
if command -v docker >/dev/null 2>&1; then
    echo "🐳 Docker available - performing full ESPHome build validation..."
    
    # Create a test configuration
    cat > test-config.yaml << 'EOF'
esphome:
  name: test_dryer_sensor

esp32:
  board: esp32dev

external_components:
  - source: components
    components: [ dryer_vent_sensor ]

dryer_vent_sensor:
  overheat:
    name: "Test Overheat"
  clog:
    name: "Test Clog"
  selftest:
    name: "Test Self Test"
EOF

    # Run ESPHome compile in Docker
    if docker run --rm -v "$(pwd):/config" -v "$(pwd)/components:/components" esphome/esphome compile test-config.yaml; then
        echo "✅ Full ESPHome build validation passed"
        rm test-config.yaml
    else
        echo "❌ ESPHome build failed - check component implementation"
        rm test-config.yaml
        exit 1
    fi
else
    echo "⚠️  Docker not available - skipping full ESPHome build validation"
    echo "ℹ️  Install Docker for complete build validation: https://docs.docker.com/get-docker/"
    echo "ℹ️  Falling back to structure validation only."
    echo "✅ ESPHome component structure validation passed"
fi

exit 0
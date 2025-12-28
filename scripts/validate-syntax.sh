#!/bin/bash

# validate-syntax.sh - Validate Python syntax in component files
# Constitution Principle VI: Validation scripts for team repeatability
# Constitution Principle VII: Host system isolation via virtual environments

set -e

echo "🔍 Validating Python syntax..."

# Check for virtual environment (Principle VII)
if [ -z "$VIRTUAL_ENV" ]; then
    echo "⚠️  No virtual environment detected. Please activate virtual environment:"
    echo "    python3 -m venv venv && source venv/bin/activate && pip install -r requirements-dev.txt"
    echo "    Then re-run this script."
    exit 1
fi

PYTHON_CMD="$VIRTUAL_ENV/bin/python"

# Check components directory
echo "Checking components/..."
find components/ -name "*.py" -type f -exec "$PYTHON_CMD" -m py_compile {} \;

# Check tests directory
echo "Checking tests/..."
find tests/ -name "*.py" -type f -exec "$PYTHON_CMD" -m py_compile {} \;

echo "✅ Python syntax validation passed"

exit 0
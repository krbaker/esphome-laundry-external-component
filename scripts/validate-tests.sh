#!/bin/bash

# validate-tests.sh - Run and validate test suite
# Constitution Principle VI: Validation scripts for team repeatability
# Constitution Principle VII: Host system isolation via virtual environments

set -e

echo "🧪 Running test suite..."

# Check for virtual environment (Principle VII)
if [ -z "$VIRTUAL_ENV" ]; then
    echo "⚠️  No virtual environment detected. Please activate virtual environment:"
    echo "    python3 -m venv venv && source venv/bin/activate && pip install -r requirements-dev.txt"
    echo "    Then re-run this script."
    exit 1
fi

# Check if pytest is available in virtual environment
PYTEST_CMD="$VIRTUAL_ENV/bin/pytest"
if ! [ -x "$PYTEST_CMD" ]; then
    echo "❌ pytest not found in virtual environment. Install with: pip install -r requirements-dev.txt"
    exit 1
fi

# Run tests with coverage if pytest-cov is available
if [ -x "$VIRTUAL_ENV/bin/pytest-cov" ]; then
    echo "Running tests with coverage..."
    "$PYTEST_CMD" --cov=components --cov-report=term-missing --cov-report=html:htmlcov
else
    echo "Running tests (coverage not available - install pytest-cov for coverage reports)..."
    "$PYTEST_CMD"
fi

echo "✅ Test validation passed"

exit 0
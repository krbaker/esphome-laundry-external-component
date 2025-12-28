import pytest
import os

def test_component_structure():
    """Test that component files exist and are readable"""
    required_files = [
        "components/dryer_vent_sensor/__init__.py",
        "components/dryer_vent_sensor/sensor.h",
        "components/dryer_vent_sensor/sensor.py"
    ]

    for filepath in required_files:
        assert os.path.exists(filepath), f"Required file {filepath} does not exist"
        with open(filepath, 'r') as f:
            content = f.read()
            assert len(content) > 0, f"File {filepath} is empty"

def test_python_syntax():
    """Test that Python files have valid syntax"""
    python_files = [
        "components/dryer_vent_sensor/__init__.py",
        "components/dryer_vent_sensor/sensor.py"
    ]

    for filepath in python_files:
        if os.path.exists(filepath):
            with open(filepath, 'r') as f:
                code = f.read()
            try:
                compile(code, filepath, 'exec')
            except SyntaxError as e:
                pytest.fail(f"Syntax error in {filepath}: {e}")

def test_config_has_required_fields():
    """Test that configuration files have required structure"""
    # This is a basic test - in a real scenario we'd validate against ESPHome schema
    init_file = "components/dryer_vent_sensor/__init__.py"
    if os.path.exists(init_file):
        with open(init_file, 'r') as f:
            content = f.read()
            # Check for basic ESPHome component structure
            assert "CONFIG_SCHEMA" in content, "CONFIG_SCHEMA not found in __init__.py"
            assert "to_code" in content, "to_code function not found in __init__.py"

def test_example_configs_exist():
    """Test that example configuration files exist"""
    examples_dir = "examples"
    if os.path.exists(examples_dir):
        yaml_files = [f for f in os.listdir(examples_dir) if f.endswith(('.yaml', '.yml'))]
        assert len(yaml_files) > 0, "No YAML example files found in examples directory"
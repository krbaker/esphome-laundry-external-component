import pytest
import os
import re

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

def test_all_original_sensors_implemented():
    """Test that all sensors from the original dryer_vent_sensor.h are implemented in the external component"""

    # Read the original dryer_vent_sensor.h file
    original_file = "dryer_vent_sensor.h"
    assert os.path.exists(original_file), f"Original spec file {original_file} not found"

    with open(original_file, 'r') as f:
        original_content = f.read()

    # Extract sensor declarations from original file
    # Pattern: Sensor *sensor_name = new Sensor();
    sensor_pattern = r'Sensor\s*\*\s*(\w+)\s*=\s*new\s*Sensor\(\)\s*;'
    original_sensors = re.findall(sensor_pattern, original_content)

    expected_sensors = [
        'overheat_sensor',
        'clog_sensor',
        'selftest_sensor',
        'short_packet_sensor',
        'short_start_packet',
        'long_start_packet',
        'short_clog_packet',
        'long_clog_packet',
        'short_overheat_packet',
        'long_overheat_packet',
        'unknown_packet_sensor',
        'selftest_counter'
    ]

    # Verify we found all expected sensors
    assert len(original_sensors) == len(expected_sensors), f"Expected {len(expected_sensors)} sensors, found {len(original_sensors)}"
    for sensor in expected_sensors:
        assert sensor in original_sensors, f"Expected sensor '{sensor}' not found in original file"

    # Check that external component __init__.py defines all sensors
    init_file = "components/dryer_vent_sensor/__init__.py"
    with open(init_file, 'r') as f:
        init_content = f.read()

    # Map original sensor names to config constants
    sensor_config_map = {
        'overheat_sensor': 'CONF_OVERHEAT',
        'clog_sensor': 'CONF_CLOG',
        'selftest_sensor': 'CONF_SELFTEST',
        'short_packet_sensor': 'CONF_SHORT_PACKET',
        'short_start_packet': 'CONF_SHORT_START_PACKET',
        'long_start_packet': 'CONF_LONG_START_PACKET',
        'short_clog_packet': 'CONF_SHORT_CLOG_PACKET',
        'long_clog_packet': 'CONF_LONG_CLOG_PACKET',
        'short_overheat_packet': 'CONF_SHORT_OVERHEAT_PACKET',
        'long_overheat_packet': 'CONF_LONG_OVERHEAT_PACKET',
        'unknown_packet_sensor': 'CONF_UNKNOWN_PACKET',
        'selftest_counter': 'CONF_SELFTEST_COUNTER'
    }

    # Check that all config constants are defined
    for config_const in sensor_config_map.values():
        assert config_const in init_content, f"Config constant {config_const} not found in __init__.py"

    # Check that all sensors are in CONFIG_SCHEMA
    for config_const in sensor_config_map.values():
        assert config_const in init_content, f"{config_const} not found in CONFIG_SCHEMA"

    # Check that all setter methods are called in to_code
    setter_method_map = {
        'overheat_sensor': 'set_overheat_sensor',
        'clog_sensor': 'set_clog_sensor',
        'selftest_sensor': 'set_selftest_sensor',
        'short_packet_sensor': 'set_short_packet_sensor',
        'short_start_packet': 'set_short_start_packet_sensor',
        'long_start_packet': 'set_long_start_packet_sensor',
        'short_clog_packet': 'set_short_clog_packet_sensor',
        'long_clog_packet': 'set_long_clog_packet_sensor',
        'short_overheat_packet': 'set_short_overheat_packet_sensor',
        'long_overheat_packet': 'set_long_overheat_packet_sensor',
        'unknown_packet_sensor': 'set_unknown_packet_sensor',
        'selftest_counter': 'set_selftest_counter_sensor'
    }

    for setter in setter_method_map.values():
        assert f"var.{setter}(sens)" in init_content, f"Setter method {setter} not called in to_code"

    # Check that C++ implementation has all setter methods
    cpp_file = "components/dryer_vent_sensor/sensor.h"
    with open(cpp_file, 'r') as f:
        cpp_content = f.read()

    for setter in setter_method_map.values():
        assert f"void {setter}" in cpp_content, f"Setter method {setter} not found in C++ implementation"

    # Check that C++ implementation has all sensor member variables
    for sensor_var in expected_sensors:
        # Convert to the namespaced version used in the C++ code
        namespaced_var = f"esphome::sensor::Sensor *{sensor_var}_"
        assert namespaced_var in cpp_content, f"Sensor member variable {sensor_var}_ not found in C++ implementation"

    print(f"✅ All {len(expected_sensors)} sensors from original specification are properly implemented:")
    for sensor in expected_sensors:
        print(f"  - {sensor}")

def test_original_sensor_publishing():
    """Test that the original sensor publishing logic is preserved"""

    # Read original file
    with open("dryer_vent_sensor.h", 'r') as f:
        original_content = f.read()

    # Extract publish_state calls from original
    publish_pattern = r'(\w+)_sensor->publish_state\((\w+)\);'
    original_publishes = re.findall(publish_pattern, original_content)

    expected_publishes = [
        ('overheat', 'overheat', False),  # boolean
        ('clog', 'clog', False),  # boolean
        ('selftest', 'test_failed', False),  # boolean
        ('short_packet', 'short_packet', True),  # cast to float
        ('short_start_packet', 'short_start', True),  # cast to float
        ('long_start_packet', 'long_start', True),  # cast to float
        ('short_clog_packet', 'short_clog', True),  # cast to float
        ('long_clog_packet', 'long_clog', True),  # cast to float
        ('short_overheat_packet', 'short_overheat', True),  # cast to float
        ('long_overheat_packet', 'long_overheat', True),  # cast to float
        ('unknown_packet', 'unknown_packet', True),  # cast to float
        ('selftest_counter', 'selftest_count', True)  # cast to float
    ]

    # Check that C++ implementation has corresponding publish calls
    cpp_file = "components/dryer_vent_sensor/sensor.h"
    with open(cpp_file, 'r') as f:
        cpp_content = f.read()

    for sensor_prefix, variable, cast_to_float in expected_publishes:
        if cast_to_float:
            # Look for pattern like: if (short_packet_sensor_) short_packet_sensor_->publish_state((float)short_packet);
            publish_call = f"if ({sensor_prefix}_sensor_) {sensor_prefix}_sensor_->publish_state((float){variable})"
        else:
            # Look for pattern like: if (overheat_sensor_) overheat_sensor_->publish_state(overheat);
            publish_call = f"if ({sensor_prefix}_sensor_) {sensor_prefix}_sensor_->publish_state({variable})"
        assert publish_call in cpp_content, f"Publish call for {sensor_prefix}_sensor not found in C++ implementation"
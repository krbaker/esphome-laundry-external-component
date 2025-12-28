# ESPHome Dryer Vent Sensor External Component

An ESPHome external component for monitoring dryer vent sensors. This component converts the original dryer vent sensor logic into a reusable ESPHome external component that maintains all original functionality while following ESPHome best practices.

## Features

- **Clog Detection**: Monitors for 3-pulse patterns indicating vent clogging
- **Overheat Detection**: Monitors for 5-pulse patterns indicating overheating
- **Self-Test**: Automatic daily self-test to verify sensor reliability
- **Packet Counters**: Tracks various packet types for diagnostics
- **Configurable Pins**: Customizable GPIO pins for pulse input and test output

## Installation

### Method 1: GitHub Source

Add to your ESPHome configuration:

```yaml
external_components:
  - source: github://your-username/dryer-vent-sensor
    components: [ dryer_vent_sensor ]

dryer_vent_sensor:
  pulse_pin: 2    # GPIO pin for pulse input (default: 2)
  test_pin: 15    # GPIO pin for test output (default: 15)

sensor:
  - platform: dryer_vent_sensor
    overheat:
      name: "Dryer Overheat Status"
    clog:
      name: "Dryer Vent Clog Status"
    selftest:
      name: "Sensor Self Test Status"
```

### Method 2: Local Source

Clone this repository and reference the local path:

```yaml
external_components:
  - source: components
    components: [ dryer_vent_sensor ]
```

## Configuration

### Component Configuration

```yaml
dryer_vent_sensor:
  pulse_pin: 2        # GPIO pin for pulse input (default: 2)
  test_pin: 15        # GPIO pin for test output (default: 15)
```

### Available Sensors

All sensors are optional - only configure the ones you need:

```yaml
sensor:
  - platform: dryer_vent_sensor
    overheat:
      name: "Dryer Overheat"
      device_class: temperature
    clog:
      name: "Dryer Vent Clog"
    selftest:
      name: "Sensor Self Test"
    short_packet:
      name: "Short Packets"
    short_start_packet:
      name: "Short Start Packets"
    long_start_packet:
      name: "Long Start Packets"
    short_clog_packet:
      name: "Short Clog Packets"
    long_clog_packet:
      name: "Long Clog Packets"
    short_overheat_packet:
      name: "Short Overheat Packets"
    long_overheat_packet:
      name: "Long Overheat Packets"
    unknown_packet:
      name: "Unknown Packets"
    selftest_counter:
      name: "Self Test Counter"
```

## Hardware Requirements

- **ESP32 Board**: Required for pulse counter peripheral
- **Pulse Input Pin**: Connected to dryer vent sensor signal
- **Test Output Pin**: Connected to sensor test trigger (optional)

## Sensor Behavior

- **overheat**: Boolean - true when 5-pulse overheat pattern detected
- **clog**: Boolean - true when 3-pulse clog pattern detected
- **selftest**: Boolean - true when self-test fails
- **Packet Counters**: Numeric - count of various packet types detected
- **selftest_counter**: Numeric - number of self-tests performed

## Self-Test Operation

- Runs automatically every 24 hours
- Triggers test signal on test_pin
- Monitors for response pulses within 3 polling cycles
- Updates selftest sensor if test fails

## Example Configuration

```yaml
esphome:
  name: dryer_monitor
  platform: ESP32
  board: esp32dev

external_components:
  - source: github://your-username/dryer-vent-sensor
    components: [ dryer_vent_sensor ]

dryer_vent_sensor:
  pulse_pin: 2
  test_pin: 15

sensor:
  - platform: dryer_vent_sensor
    overheat:
      name: "Dryer Overheat Alert"
      device_class: temperature
    clog:
      name: "Dryer Vent Clog Alert"
    selftest:
      name: "Sensor Health Check"

binary_sensor:
  - platform: template
    name: "Dryer Needs Attention"
    lambda: |-
      return id(overheat_alert).state || id(clog_alert).state;
```

## Troubleshooting

- **No sensor updates**: Check GPIO pin connections and ESP32 board compatibility
- **Self-test failures**: Verify test_pin connection and sensor responsiveness
- **Incorrect pulse detection**: Ensure pulse_pin is connected to correct sensor output
- **Build errors**: Confirm ESPHome version compatibility and external component source

## Development

This component is developed following ESPHome external component guidelines:

- C++ implementation in `components/dryer_vent_sensor/sensor.h`
- Python configuration in `components/dryer_vent_sensor/__init__.py`
- Unit tests in `tests/`

### Development Setup

1. Create a virtual environment:
   ```bash
   python3 -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

2. Install development dependencies:
   ```bash
   pip install -r requirements-dev.txt
   ```

3. Run validation scripts:
   ```bash
   ./scripts/validate-syntax.sh
   ./scripts/validate-build.sh
   ./scripts/validate-tests.sh
   ```

### Host System Isolation

Following Constitution Principle VII, this project:
- Uses virtual environments for all Python tooling
- Avoids system-wide package installations
- Maintains reproducible development environments
- Prefers lightweight virtual environments over containers for local development

## License

Apache-2.0

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes following ESPHome coding standards
4. Add tests for new functionality
5. Submit a pull request
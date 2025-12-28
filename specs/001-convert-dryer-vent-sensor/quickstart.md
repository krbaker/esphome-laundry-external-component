# Quickstart: Dryer Vent Sensor External Component

## Installation

1. Add external component to your ESPHome configuration:

```yaml
external_components:
  - source: github://username/dryer-vent-sensor
    components: [ dryer_vent_sensor ]
```

2. Configure the component (optional GPIO pin configuration):

```yaml
dryer_vent_sensor:
  pulse_pin: 2    # GPIO pin for pulse input (default: 2)
  test_pin: 15    # GPIO pin for test output (default: 15)
```

3. Add sensors to monitor:

```yaml
sensor:
  - platform: dryer_vent_sensor
    overheat:
      name: "Dryer Overheat"
    clog:
      name: "Dryer Vent Clog"
    selftest:
      name: "Sensor Self Test"
```

## Hardware Setup

- Connect dryer vent sensor signal to configured pulse_pin (default GPIO 2)
- Connect test trigger to configured test_pin (default GPIO 15)
- Ensure ESP32 board supports pulse counter peripheral

## Testing

1. Compile and upload to ESP32
2. Monitor sensor values in Home Assistant
3. Trigger self-test (happens automatically every 24 hours)
4. Verify pulse detection with dryer vent hardware

## Troubleshooting

- Check configured GPIO pin connections (pulse_pin and test_pin)
- Verify ESP32 board compatibility
- Monitor ESPHome logs for pulse counter errors
- Ensure no other components use the configured GPIO pins
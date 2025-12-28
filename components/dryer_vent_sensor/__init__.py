import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)

AUTO_LOAD = ["sensor"]
CODEOWNERS = ["@your-username"]
DEPENDENCIES = []

dryer_vent_sensor_ns = cg.esphome_ns.namespace("dryer_vent_sensor")
DryerVentSensor = dryer_vent_sensor_ns.class_("DryerVentSensor", cg.PollingComponent)

CONF_PULSE_PIN = "pulse_pin"
CONF_TEST_PIN = "test_pin"
CONF_OVERHEAT = "overheat"
CONF_CLOG = "clog"
CONF_SELFTEST = "selftest"
CONF_SHORT_PACKET = "short_packet"
CONF_SHORT_START_PACKET = "short_start_packet"
CONF_LONG_START_PACKET = "long_start_packet"
CONF_SHORT_CLOG_PACKET = "short_clog_packet"
CONF_LONG_CLOG_PACKET = "long_clog_packet"
CONF_SHORT_OVERHEAT_PACKET = "short_overheat_packet"
CONF_LONG_OVERHEAT_PACKET = "long_overheat_packet"
CONF_UNKNOWN_PACKET = "unknown_packet"
CONF_SELFTEST_COUNTER = "selftest_counter"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DryerVentSensor),
        cv.Optional(CONF_PULSE_PIN, default=2): cv.int_,
        cv.Optional(CONF_TEST_PIN, default=15): cv.int_,
        cv.Optional(CONF_OVERHEAT): sensor.sensor_schema(
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            unit_of_measurement=UNIT_CELSIUS,
        ),
        cv.Optional(CONF_CLOG): sensor.sensor_schema(),
        cv.Optional(CONF_SELFTEST): sensor.sensor_schema(),
        cv.Optional(CONF_SHORT_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_SHORT_START_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_LONG_START_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_SHORT_CLOG_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_LONG_CLOG_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_SHORT_OVERHEAT_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_LONG_OVERHEAT_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_UNKNOWN_PACKET): sensor.sensor_schema(),
        cv.Optional(CONF_SELFTEST_COUNTER): sensor.sensor_schema(),
    }
).extend(cv.polling_component_schema("15s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_pulse_pin(config[CONF_PULSE_PIN]))
    cg.add(var.set_test_pin(config[CONF_TEST_PIN]))

    if CONF_OVERHEAT in config:
        sens = await sensor.new_sensor(config[CONF_OVERHEAT])
        cg.add(var.set_overheat_sensor(sens))
    if CONF_CLOG in config:
        sens = await sensor.new_sensor(config[CONF_CLOG])
        cg.add(var.set_clog_sensor(sens))
    if CONF_SELFTEST in config:
        sens = await sensor.new_sensor(config[CONF_SELFTEST])
        cg.add(var.set_selftest_sensor(sens))
    if CONF_SHORT_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_SHORT_PACKET])
        cg.add(var.set_short_packet_sensor(sens))
    if CONF_SHORT_START_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_SHORT_START_PACKET])
        cg.add(var.set_short_start_packet_sensor(sens))
    if CONF_LONG_START_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_LONG_START_PACKET])
        cg.add(var.set_long_start_packet_sensor(sens))
    if CONF_SHORT_CLOG_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_SHORT_CLOG_PACKET])
        cg.add(var.set_short_clog_packet_sensor(sens))
    if CONF_LONG_CLOG_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_LONG_CLOG_PACKET])
        cg.add(var.set_long_clog_packet_sensor(sens))
    if CONF_SHORT_OVERHEAT_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_SHORT_OVERHEAT_PACKET])
        cg.add(var.set_short_overheat_packet_sensor(sens))
    if CONF_LONG_OVERHEAT_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_LONG_OVERHEAT_PACKET])
        cg.add(var.set_long_overheat_packet_sensor(sens))
    if CONF_UNKNOWN_PACKET in config:
        sens = await sensor.new_sensor(config[CONF_UNKNOWN_PACKET])
        cg.add(var.set_unknown_packet_sensor(sens))
    if CONF_SELFTEST_COUNTER in config:
        sens = await sensor.new_sensor(config[CONF_SELFTEST_COUNTER])
        cg.add(var.set_selftest_counter_sensor(sens))
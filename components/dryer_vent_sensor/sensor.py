import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_PULSE_PIN, CONF_TEST_PIN, DryerVentSensor, dryer_vent_sensor_ns

DEPENDENCIES = ["dryer_vent_sensor"]

DryerVentSensorPlatform = dryer_vent_sensor_ns.class_(
    "DryerVentSensorPlatform", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(DryerVentSensorPlatform),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
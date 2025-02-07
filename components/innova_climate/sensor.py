import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
import Innova, CONF_INNOVA_ID, innova_ns
from esphome.const import (
    CONF_TEMPERATURE,
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_REVOLUTIONS_PER_MINUTE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    ICON_FAN,

)

DEPENDENCIES = ['innova']

CONF_WATER_TEMPERATURE = "water_temperature"
CONF_AIR_TEMPERATURE = "air_temperature"
CONF_FAN_SPEED = "fan_speed"
CONF_SETPOINT = "setpoint"


CONFIG_SCHEMA = {
            cv.GenerateID(CONF_INNOVA_ID): cv.use_id(Innova),
            cv.Optional(CONF_AIR_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WATER_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_FAN_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_REVOLUTIONS_PER_MINUTE,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_FAN,
            ),
            cv.Optional(CONF_SETPOINT): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            )
        }


async def to_code(config):
    
    var = await cg.get_variable(config[CONF_INNOVA_ID])

    if CONF_AIR_TEMPERATURE in config:
        conf = config[CONF_AIR_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_air_temperature_sensor(sens))
    if CONF_WATER_TEMPERATURE in config:
        conf = config[CONF_WATER_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_water_temperature_sensor(sens))
    if CONF_FAN_SPEED in config:
        conf = config[CONF_FAN_SPEED]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_fan_speed_sensor(sens))
    if CONF_SETPOINT in config:
        conf = config[CONF_SETPOINT]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_setpoint_sensor(sens))

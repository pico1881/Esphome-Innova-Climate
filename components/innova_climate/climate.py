import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, modbus, sensor, binary_sensor

from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_EMPTY,
    ENTITY_CATEGORY_NONE,
    STATE_CLASS_MEASUREMENT,
    UNIT_REVOLUTIONS_PER_MINUTE,
    UNIT_CELSIUS,
    ICON_FAN,
)

AUTO_LOAD = ['modbus', 'sensor', 'binary_sensor']

innova_ns = cg.esphome_ns.namespace("innova")

Innova = innova_ns.class_("Innova", climate.Climate, cg.PollingComponent, modbus.ModbusDevice)

CONF_INNOVA_ID = 'innova_id'
CONF_WATER_TEMPERATURE = "water_temperature"
CONF_AIR_TEMPERATURE = "air_temperature"
CONF_FAN_SPEED = "fan_speed"
CONF_SETPOINT = "setpoint"
CONF_BOILER_RELAY = "boiler_relay"
CONF_CHILLER_RELAY = "chiller_relay"

CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Innova),
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
            ),
            cv.Optional(CONF_BOILER_RELAY): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_EMPTY,
                entity_category=ENTITY_CATEGORY_NONE,
            ),
            cv.Optional(CONF_CHILLER_RELAY): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_EMPTY,
                entity_category=ENTITY_CATEGORY_NONE,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(modbus.modbus_device_schema(0x01))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    await modbus.register_modbus_device(var, config)

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
    if CONF_BOILER_RELAY in config:
        conf = config[CONF_BOILER_RELAY]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_boiler_relay_sensor(sens))
    if CONF_CHILLER_RELAY in config:
        conf = config[CONF_CHILLER_RELAY]        
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_chiller_relay_sensor(sens))

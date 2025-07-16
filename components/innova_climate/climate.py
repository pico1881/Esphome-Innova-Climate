import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, modbus, sensor, switch

from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    ICON_FAN,
)

CODEOWNERS = ["@pico1881"]
AUTO_LOAD = ['modbus', 'sensor', "switch"]

innova_ns = cg.esphome_ns.namespace("innova")
Innova = innova_ns.class_("Innova", climate.Climate, cg.PollingComponent, modbus.ModbusDevice)
InnovaSwitch = innova_ns.class_("InnovaSwitch", switch.Switch)

CONF_INNOVA_ID = 'innova_id'

CONF_AIR_TEMPERATURE = "air_temperature"

CONF_SETPOINT = "setpoint"
CONF_ALARM = "alarm"
CONF_KEY_LOCK_SWITCH = "key_lock_switch"

KEY_LOCK_SCHEMA = switch.SWITCH_SCHEMA.extend(
    {cv.GenerateID(CONF_ID): cv.declare_id(InnovaSwitch)}
)


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
            cv.Optional(CONF_SETPOINT): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ALARM): sensor.sensor_schema(
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
            ),
            cv.Optional(CONF_KEY_LOCK_SWITCH): KEY_LOCK_SCHEMA
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
    if CONF_SETPOINT in config:
        conf = config[CONF_SETPOINT]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_setpoint_sensor(sens))
    if CONF_ALARM in config:
        conf = config[CONF_ALARM]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_alarm_sensor(sens))        
    if CONF_KEY_LOCK_SWITCH in config: 
        conf = config[CONF_KEY_LOCK_SWITCH]
        swt = await switch.new_switch(conf)
        cg.add(var.set_key_lock_switch(swt))
        await cg.register_parented(swt, var)

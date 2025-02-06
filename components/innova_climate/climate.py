import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor, modbus

from esphome.const import (
    CONF_ID, 
)

AUTO_LOAD = ["modbus"]

innova_ns = cg.esphome_ns.namespace("innova")

Innova = innova_ns.class_("Innova", climate.Climate, cg.PollingComponent, modbus.ModbusDevice)

CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Innova),
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

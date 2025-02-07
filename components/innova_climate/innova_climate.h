#pragma once

#include "esphome/components/modbus/modbus.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/helpers.h"
#include <deque>

namespace esphome {
namespace innova {

struct WriteableData
{
  uint8_t function_value;
  uint16_t register_value;
  uint16_t write_value;
};

class Innova : public esphome::climate::Climate, public PollingComponent, public modbus::ModbusDevice {
 public:
  void set_air_temperature_sensor(sensor::Sensor *air_temperature_sensor) { air_temperature_sensor_ = air_temperature_sensor; }
  void set_water_temperature_sensor(sensor::Sensor *water_temperature_sensor) { water_temperature_sensor_ = water_temperature_sensor; }
  void set_fan_speed_sensor(sensor::Sensor *fan_speed_sensor) { fan_speed_sensor_ = fan_speed_sensor; }
  void set_setpoint_sensor(sensor::Sensor *setpoint_sensor) { setpoint_sensor_ = setpoint_sensor; }
  void set_boiler_relay_sensor(binary_sensor::BinarySensor *boiler_relay_sensor) { boiler_relay_sensor_ = boiler_relay_sensor; }
  void set_chiller_relay_sensor(binary_sensor::BinarySensor *chiller_relay_sensor) { chiller_relay_sensor_ = chiller_relay_sensor; }
  void setLockSwitch(switch_::Switch *lockSwitch)
      {
        this->lockSwitch = lockSwitch;
      };
  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void add_to_queue(uint8_t function, uint8_t new_value, uint16_t address);
  
  void setLock(bool state);

  climate::ClimateTraits traits() override {
    auto traits = climate::ClimateTraits();
    traits.set_supports_action(true);
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({
           climate::CLIMATE_MODE_OFF, 
           climate::ClimateMode::CLIMATE_MODE_HEAT,
           climate::ClimateMode::CLIMATE_MODE_COOL
    });
    traits.set_visual_min_temperature(16.0);
    traits.set_visual_max_temperature(28.0);
    traits.set_visual_target_temperature_step(0.5);
    traits.set_visual_current_temperature_step(0.1);
    traits.set_supported_fan_modes({
            climate::CLIMATE_FAN_AUTO,
            climate::CLIMATE_FAN_LOW,
            climate::CLIMATE_FAN_MEDIUM,
            climate::CLIMATE_FAN_HIGH,
    });
    return traits;
  }
private:
switch_::Switch *lockSwitch;

 protected:
  int state_{0};
  bool waiting_{false};
  uint32_t last_send_{0};
  bool waiting_for_write_ack_{false};
  int fan_speed_;
  int program_;
  int season_;
  std::deque<WriteableData>writequeue_;
  void writeModbusRegister(WriteableData write_data);

  void control(const climate::ClimateCall &call) override; 

  sensor::Sensor *air_temperature_sensor_{nullptr};
  sensor::Sensor *water_temperature_sensor_{nullptr};
  sensor::Sensor *fan_speed_sensor_{nullptr};
  sensor::Sensor *setpoint_sensor_{nullptr};
  binary_sensor::BinarySensor *boiler_relay_sensor_{nullptr};
  binary_sensor::BinarySensor *chiller_relay_sensor_{nullptr};

};

}  // namespace innova
}  // namespace esphome

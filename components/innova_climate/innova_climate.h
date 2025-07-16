#pragma once

#include "esphome/components/modbus/modbus.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
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
  void set_alarm_sensor(sensor::Sensor *alarm_sensor) { alarm_sensor_ = alarm_sensor; }
  void set_setpoint_sensor(sensor::Sensor *setpoint_sensor) { setpoint_sensor_ = setpoint_sensor; }

  void set_key_lock_switch(switch_::Switch *key_lock_switch) { key_lock_switch_ = key_lock_switch; }

  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void add_to_queue(uint8_t function, uint8_t new_value, uint16_t address);
  void set_key_lock(bool state);

  climate::ClimateTraits traits() override {
    auto traits = climate::ClimateTraits();
    traits.set_supports_action(false);
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

 protected:
  int state_{0};
  bool waiting_{false};
  uint32_t last_send_{0};
  bool waiting_for_write_ack_{false};
  int alarm_{0};
  int program_{0};
  int season_{0};
  std::deque<WriteableData>writequeue_;
  void write_modbus_register(WriteableData write_data);

  void control(const climate::ClimateCall &call) override; 

  sensor::Sensor *air_temperature_sensor_{nullptr};
  sensor::Sensor *alarm_sensor_{nullptr};

  sensor::Sensor *setpoint_sensor_{nullptr};

  switch_::Switch *key_lock_switch_{nullptr};
};

}  // namespace innova
}  // namespace esphome

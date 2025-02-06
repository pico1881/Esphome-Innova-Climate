// Example: innova.h

#pragma once

#include "esphome/components/modbus/modbus.h"
#include "esphome/components/climate/climate.h"
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
  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void add_to_queue(uint8_t function, float new_value, uint16_t address);

  climate::ClimateTraits traits() override {
    // Return the traits of this climate device.
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

 protected:
  int state_{0};
  bool waiting_{false};
  //bool write_data_{false};
  uint32_t last_send_{0};
  bool waiting_for_write_ack_{false};
  float current_temp_{10.0};
  float target_temp_{10.0};
 // float water_temp_;
  int fan_speed_;
  int program_;
  int season_;
  enum ReadWriteMode {
    read,
    write,
    idle
  };
  ReadWriteMode current_read_write_mode_ = { Innova::read };
  std::deque<WriteableData>writequeue_;
  void writeModbusRegister(WriteableData write_data);

  void control(const climate::ClimateCall &call) override; 
};

}  // namespace innova
}  // namespace esphome

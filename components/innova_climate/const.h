#pragma once

namespace esphome {
namespace innova {


static const uint8_t CMD_READ_REG = 0x03;
static const uint8_t CMD_WRITE_REG = 0x06;
static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00;    // reg 0
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x01;  // reg 1
static const uint16_t INNOVA_FAN_SPEED = 0x0F;          // reg 15
static const uint16_t INNOVA_PROGRAM = 0xC9;            // reg 201
static const uint16_t INNOVA_SEASON = 0xE9;             // reg 233
static const uint16_t INNOVA_SETPOINT = 0xE7;           // reg 231
static const uint16_t INNOVA_OUT = 0x09;           	// reg 9

}  // namespace innova
}  // namespace esphome

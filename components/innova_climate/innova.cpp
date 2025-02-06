#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";
static const uint8_t CMD_READ_REG = 0x03;
static const uint8_t CMD_WRITE_REG = 0x06;
static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00;    // reg 0
static const uint16_t INNOVA_FAN_SPEED = 0x0F;          // reg 15
static const uint16_t INNOVA_PROGRAM = 0xC9;            // reg 201
static const uint16_t INNOVA_SEASON = 0xE9;             // reg 233
static const uint16_t INNOVA_SETPOINT = 0xE7;           // reg 231
static const uint16_t REGISTER[] = {INNOVA_AIR_TEMPERATURE, INNOVA_SETPOINT, INNOVA_FAN_SPEED, INNOVA_PROGRAM, INNOVA_SEASON};

void Innova::setup() {}

void Innova::on_modbus_data(const std::vector<uint8_t> &data) {
    this->waiting_ = false;
    if (!waiting_for_write_ack_ && data.size() < 2) {
        ESP_LOGW(TAG, "Invalid data packet size (%d) for state %d", data.size(), this->state_);
        return;
    }
    //  Command response is 4 bytes echoing the write command
    if (waiting_for_write_ack_ )  {
	waiting_for_write_ack_ = false ; 
	if (data.size() == 4) {
		ESP_LOGD(TAG, "Write command succeeded");
	} else {
		ESP_LOGW(TAG, "Invalid data packet size (%d) while waiting for write command response", data.size());
	}
	return ; 
    }

     //ESP_LOGD(TAG, "Data: %s", format_hex_pretty(data).c_str());  

    auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };

    int value = get_16bit(0);

    switch (this->state_) {
        case 1:
            value /= 10.0;
            this->current_temperature = value;
	ESP_LOGD(TAG, "temp=%d", value);
        break;
        case 2:
            value /= 10.0;
            this->target_temperature = value;    
        break;
        case 3:
            this->fan_speed_ = value;   
        break;
        case 4:
            this->program_ = value;   
            climate::ClimateFanMode fmode;
            switch ((int) value & 0b111) {
                case 0: fmode = climate::CLIMATE_FAN_AUTO; break;
                case 1: fmode = climate::CLIMATE_FAN_MEDIUM; break;
                case 2: fmode = climate::CLIMATE_FAN_LOW; break;
                case 3: fmode = climate::CLIMATE_FAN_HIGH; break;
                default: fmode = climate::CLIMATE_FAN_MEDIUM; break;
            }
            this->fan_mode = fmode;    
           // ESP_LOGD(TAG, "Program=%d", this->program_);
        break;
        case 5:
            this->season_ = value;   
            if (this->season_ == 3 && !((this->program_ & (0x0080)) == 128)) {
                this->mode = climate::CLIMATE_MODE_HEAT;
            } else if (this->season_ == 5 && !((this->program_ & (0x0080)) == 128)) {
                this->mode = climate::CLIMATE_MODE_COOL;
            } else {
                this->mode = climate::CLIMATE_MODE_OFF;
            }
            
            if (this->season_ == 3 && this->fan_speed_ > 0) {
                this->action = climate::CLIMATE_ACTION_HEATING;
            } else if (this->season_ == 5 && this->fan_speed_ > 0) {
                 this->action = climate::CLIMATE_ACTION_COOLING;
            } else if ((this->program_ & (0x0080)) == 128) {
                this->action = climate::CLIMATE_ACTION_OFF;	
            } else {
                this->action = climate::CLIMATE_ACTION_IDLE;  
            }
        break;
    }
    if (++this->state_ > 5){
        this->state_ = 0;
    	this->publish_state();
    }
}

void Innova::loop() {
    uint32_t now = millis();

    // timeout after 15 seconds
    if (this->waiting_ && (now - this->last_send_ > 15000)) {
        ESP_LOGW(TAG, "Timed out waiting for response");
        this->waiting_ = false;
    }
	
    if (this->waiting_ || (this->state_ == 0) ) return;

    if (this->writequeue_.size() > 0) {
        //ESP_LOGD(TAG, "Write mode: Write queue size is now: %d",this->writequeue_.size());
        writeModbusRegister(this->writequeue_.front());
        this->writequeue_.pop_front();
    } else {
        send(CMD_READ_REG, REGISTER[this->state_ - 1], 1);        
    }
    
    this->last_send_ = now;
    this->waiting_ = true;	
}

void Innova::update() {
    this->state_ = 1;
}

void Innova::add_to_queue(uint8_t function, float new_value, uint16_t address) {
    WriteableData data;
    data.function_value = function;
    data.write_value = new_value;
    data.register_value = address;
    writequeue_.emplace_back(data);
    //ESP_LOGD(TAG, "Data write pending: function (%i), value (%i), address (%i)", data.function_value, data.write_value, data.register_value);
}

void Innova::writeModbusRegister(WriteableData write_data) { 
    uint8_t payload[] = {(uint8_t)(write_data.write_value >> 8), (uint8_t)write_data.write_value };
    send( write_data.function_value,write_data.register_value,1,sizeof(payload),payload);
    this->waiting_for_write_ack_ = true ;
}

void Innova::control(const climate::ClimateCall &call) {
    if (call.get_mode().has_value()) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;
        this->mode = *call.get_mode();
        climate::ClimateMode mode = *call.get_mode();
        switch (mode) {
            case climate::CLIMATE_MODE_OFF:
                //ESP_LOGD(TAG, "Set Climate Mode: OFF");
                new_prg = curr_prg | (1 << 7);
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
		
            break;
            case climate::CLIMATE_MODE_HEAT:
                //ESP_LOGD(TAG, "Set Climate Mode: HEAT");
                add_to_queue(CMD_WRITE_REG,3, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);  
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;
            case climate::CLIMATE_MODE_COOL:
                //ESP_LOGD(TAG, "Set Climate Mode:COOL");
                add_to_queue(CMD_WRITE_REG,5, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;
            default: 
                ESP_LOGW(TAG, "Unsupported mode: %d", mode); 
            break;
        }
    }

    if (call.get_fan_mode().has_value()) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;
        this->fan_mode = *call.get_fan_mode();
        climate::ClimateFanMode fan_mode = *call.get_fan_mode();
        switch (fan_mode) {
            case climate::CLIMATE_FAN_LOW:
                new_prg = (curr_prg & ~(0b111)) | 2; 
            break;
            case climate::CLIMATE_FAN_MEDIUM: 
                new_prg = (curr_prg & ~(0b111)) | 1; 
            break;
            case climate::CLIMATE_FAN_HIGH:
                new_prg = (curr_prg & ~(0b111)) | 3;
            break;
            case climate::CLIMATE_FAN_AUTO: 
                new_prg = (curr_prg & ~(0b111)); 
            break;
            default: 
                new_prg = (curr_prg & ~(0b111)) | 1;
            break;
        }
        add_to_queue(CMD_WRITE_REG, new_prg, INNOVA_PROGRAM);
    }
    
    if (call.get_target_temperature().has_value()) {
        // User requested target temperature change
        this->target_temperature = *call.get_target_temperature();
        float target = *call.get_target_temperature() * 10.0;
        add_to_queue(CMD_WRITE_REG,target, INNOVA_SETPOINT);
    }
    this->publish_state();
    this->state_ = 1;
}

void Innova::dump_config() { 
    LOG_CLIMATE("", "Innova Climate", this); 
    ESP_LOGCONFIG(TAG, "INNOVA:");
    ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
}

}  // namespace innova
}  // namespace esphome

# Esphome-Innova-Climate
The ``innova_climate`` climate platform creates a climate device which can be used
to control a Innova fancoil via modbus communication. 

![test_image](/images/innova_climate.jpg)

This component supports the following functionality:
- Set the operating mode: off, heat, cool
- Set the desired fan speed: auto, high (maximum), medium (silent), low (night)
- Set the desired target temperature

Optional you can set sensors:
- Current temperature
- Target temperature
- Water temperature
- Fan speed sensor
  
binary sensors:
- Output boiler relay status
- Output chiller relay status

and a switch:
- Keyboard lock switch for lock the on-board keyboard (same function when you push + and - key on the device)

Hardware setup
--------------
Tested with Innova INN-FR-B32 board, can work with same board installed on other brand fancoil, for more details read the PDF file.
--------------

You need an RS485 transceiver module:

![rs485_module](/images/rs485_module.jpg)

a DC-DC step down regulator like this with the output voltage regulated at 5V from 12V input:

![voltage_regulator](/images/voltage_regulator.jpg)

Connect all as this schematic, don't forget to apply jumper at RTU pin on the Innova board:
![connection_schema](/images/connection_schema.jpg)

### Software

To use this component in your ESPHome configuration, follow the example below:

#### Example configuration

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/pico1881/Esphome-Innova-Climate
      ref: main
    components: [ innova_climate ]

logger:
  baud_rate: 0  #disable serial logger when use serial hardware pins

uart:
  tx_pin: GPIO01
  rx_pin: GPIO03
  baud_rate: 9600
  stop_bits: 1

climate:
  - platform: innova_climate
    name: Innova         #Required field, set your desired name for the climate
    address: 1           #Set modbus address of Innova board default 1
    update_interval: 60s #The interval that the sensors should be checked. Defaults to 60 seconds.
    
    #optional Air temperature sensor
    air_temperature:
      name: ${name} air temp
    #optional Water temperature sensor
    water_temperature:
      name: ${name} water temp   
    #optional Fan Speed sensor
    fan_speed:
      name: ${name} fan speed
    #optional Temperature setpoint sensor    
    setpoint:
      name: ${name} setpoint   
    #optional binary sensor for output status of boiler relay 
    boiler_relay:
      name: ${name} boiler relay
    #optional binary sensor for output status of chiller relay 
    chiller_relay:    
      name: ${name} chiller relay
    #optional switch for lock the on-board device keyboard 
    key_lock_switch:
      name: ${name} key lock
```

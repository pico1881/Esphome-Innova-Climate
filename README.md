# Esphome-Innova-Climate
The ``innova_climate`` climate platform creates a climate device which can be used
to control a Innova fancoil via modbus communication. 

This component supports the following functionality:
- Set the operating mode: off, heat, cool
- Set the desired fan speed: auto, high (maximum), medium (silent), low (night)
- Set the desired target temperature

Optional you can set these sensor:
- Current temperature
- Target temperature
- Water temperature
- Fan speed sensor
  
these binary sensor:
- Output boiler relay status
- Output chiller relay status

and this switch:
- Keyboard lock switch for lock the on-board keyboard (same function when you push + and - key on the device)

Hardware setup
--------------
You need an RS485 transceiver module:
.. figure:: /images/rs485_module.jpg

a DC-DC step down regulator like this with the output voltage regulated at 5V from 12V input:
.. figure:: /images/voltage_regulator.jpg
Connect all as this schematic:
.. figure:: /images/connection_schema.jpg
don't forget to apply jumper at RTU pin on the Innova board

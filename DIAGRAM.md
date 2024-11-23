   +3.3V/5V  ---->  +------------------------+         +------------------------+
                     |                        |         |                        |
                     |   Pull-up Resistor     |         |   Pull-up Resistor     |
                     |        (4.7kΩ)         |         |        (4.7kΩ)         |
                     |                        |         |                        |
                     +------------------------+         +------------------------+
                      |                         |          |                        |
                      |                         |          |                        |
                SDA (Pin 18)  ------------>  SDA (VL53L1X)    SCL (Pin 17) ------------>  SCL (VL53L1X)


Esp32 expansion board:
Ground   P27 
3.3 Volts P14

SCL 17 P35
SDA 18 P34
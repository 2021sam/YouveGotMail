#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <BH1750.h>  // Include the BH1750 library

class LightSensor {
public:
    // LightSensor();  // Constructor
    // Constructor that accepts an I2C address for the sensor
    LightSensor(uint8_t address);
    bool begin();   // Initialize the sensor
    bool isOnline();  // Check if the sensor is online
    void setOnlineStatus(bool status);  // Set the sensor's online status manually (optional)
    float getLightLevel(); // Get the light level in lux

private:
    BH1750 lightMeter;  // BH1750 light sensor object
    uint8_t i2cAddress;  // Store the I2C address
    bool onlineStatus;  // Track the sensor's online status
};

#endif

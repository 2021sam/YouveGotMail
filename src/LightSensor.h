#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <BH1750.h>  // Include the BH1750 library

class LightSensor {
public:
    // Constructor that accepts I2C address, SDA, SCL pins, and clock speed
    // LightSensor(uint8_t address, uint8_t sdaPin, uint8_t sclPin, uint32_t clockSpeed);
    // Constructor with optional SDA, SCL, and clock speed parameters
    LightSensor(uint8_t address, uint8_t sdaPin = 18, uint8_t sclPin = 17, uint32_t clockSpeed = 50000);


    bool begin();   // Initialize the sensor
    bool isOnline();  // Check if the sensor is online
    void setOnlineStatus(bool status);  // Set the sensor's online status manually (optional)
    float getLightLevel(); // Get the light level in lux

private:
    BH1750 lightMeter;  // BH1750 light sensor object
    uint8_t i2cAddress;  // Store the I2C address
    bool onlineStatus;  // Track the sensor's online status
    uint8_t sdaPin, sclPin; // I2C pin configuration
    uint32_t clockSpeed; // I2C clock speed
};

#endif

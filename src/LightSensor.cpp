#include "LightSensor.h"
#include <Wire.h>

// Constructor
// LightSensor::LightSensor() : onlineStatus(false) {
//     // Optional: Any initialization if needed
// }


// Constructor with custom I2C address
LightSensor::LightSensor(uint8_t address) : onlineStatus(false), i2cAddress(address) {}



// // Initialize the BH1750 light sensor
// bool LightSensor::begin() {
//     if (lightMeter.begin()) {
//         onlineStatus = true;  // Set the sensor as online
//         Serial.println("Light sensor initialized successfully.");
//         return true;
//     } else {
//         onlineStatus = false;  // Set the sensor as offline
//         Serial.println("Light sensor initialization failed.");
//         return false;
//     }
// }



// Initialize the BH1750 light sensor with a custom I2C address
bool LightSensor::begin() {
    Wire.begin();  // Initialize I2C bus

    // Initialize BH1750 with the specified I2C address
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, i2cAddress)) {
        onlineStatus = true;
        Serial.println("Light sensor initialized successfully.");
        return true;
    } else {
        onlineStatus = false;
        Serial.println("Light sensor initialization failed.");
        return false;
    }
}





// Get the light level (lux)
float LightSensor::getLightLevel() {
    // If the sensor is offline, return an error value (e.g., -1.0)
    if (!onlineStatus) {
        Serial.println("Error: Light sensor is offline.");
        return -1.0;  // Return -1.0 to indicate the sensor is offline
    }

    // Read the light level (lux)
    float lux = lightMeter.readLightLevel();
    
    // Check if the reading is valid
    if (lux < 0) {
        Serial.println("Error: Invalid light sensor reading.");
        return -1.0;  // Return -1.0 for invalid data
    }

    return lux;  // Return the light level in lux
}

// Check if the sensor is online
bool LightSensor::isOnline() {
    return onlineStatus;
}

// Set the sensor's online status manually (useful for external checks)
void LightSensor::setOnlineStatus(bool status) {
    onlineStatus = status;
}

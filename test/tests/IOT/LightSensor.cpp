#include "LightSensor.h"
#include <Wire.h>

// Constructor
LightSensor::LightSensor() {
    // Optional: Any initialization if needed
}

// Initialize the BH1750 light sensor
bool LightSensor::begin() {
    if (lightMeter.begin()) {
        Serial.println("Light sensor initialized successfully.");
        return true;
    } else {
        Serial.println("Light sensor initialization failed.");
        return false;
    }
}

// Get the light level (lux)
float LightSensor::getLightLevel() {
    return lightMeter.readLightLevel();  // Read the light level in lux
}

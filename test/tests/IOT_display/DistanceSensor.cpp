#include "DistanceSensor.h"
#include <Wire.h>

// Constructor
// DistanceSensor::DistanceSensor() : onlineStatus(false) {
//     // Optional: Add any initialization if needed
// }


// Constructor
DistanceSensor::DistanceSensor(uint8_t address) : onlineStatus(false), i2cAddress(address) {
    // Optionally initialize any member variables if needed
}

// Initialize the VL53L1X distance sensor and check if it is online
bool DistanceSensor::begin() {
    Wire.begin();  // Initialize the I2C bus
    
    bool initStatus = distanceSensor.begin();  // Get the return value of begin()
    Serial.print("distanceSensor.begin() = ");
    Serial.println(initStatus);
    // Attempt to initialize the sensor and set the I2C address
    if (!initStatus) {
        distanceSensor.setI2CAddress(i2cAddress);  // Set the I²C address
        onlineStatus = true;  // If successful, set status to online
        return true;  // Sensor initialized successfully
    } else {
        onlineStatus = false;  // If failed, set status to offline
        return false;  // Initialization failed
    }
}



// Set a new I²C address
bool DistanceSensor::setAddress(uint8_t newAddress) {
    // You can use this function to change the address at any time
    if (newAddress >= 0x08 && newAddress <= 0x77) {  // Valid address range
        i2cAddress = newAddress;  // Store the new address
        distanceSensor.setI2CAddress(newAddress);  // Set the new address on the sensor
        return true;
    }
    return false;  // Return false if the address is out of range
}



// Check if the sensor is online (initialized and working)
bool DistanceSensor::isOnline() {
    return onlineStatus;  // Return the current status of the sensor
}


// Get the distance in centimeters
float DistanceSensor::getDistance() {
    if (!onlineStatus) {  // Check if the sensor is online
        return -1.0;  // Return -1 if the sensor is offline
    }

    distanceSensor.startRanging();  // Start ranging
    while (!distanceSensor.checkForDataReady()) {
        delay(1);  // Wait for data
    }

    // unsigned long startTime = millis();
    // while (!distanceSensor.checkForDataReady()) {
    //     // Timeout after 1000ms (1 second) if no data is ready
    //     if (millis() - startTime > 1000) {
    //         distanceSensor.stopRanging();  // Stop ranging if timeout occurs
    //         return -1.0;  // Return -1 if data was not ready in time
    //     }
    //     delay(1);  // Wait for data to be ready
    // }

    int distanceInMM = distanceSensor.getDistance();  // Distance in mm
    distanceSensor.clearInterrupt();  // Clear interrupt
    distanceSensor.stopRanging();  // Stop ranging

    return distanceInMM / 10.0;  // Convert mm to cm and return
}

// Expose the necessary methods from SFEVL53L1X

// Start ranging operation
void DistanceSensor::startRanging() {
    if (onlineStatus) {
        distanceSensor.startRanging();
    }
}

// Check if the data is ready
bool DistanceSensor::checkForDataReady() {
    if (onlineStatus) {
        return distanceSensor.checkForDataReady();
    }
    return false;
}


// Clear interrupt if necessary
void DistanceSensor::clearInterrupt() {
    if (onlineStatus) {
        distanceSensor.clearInterrupt();
    }
}


void DistanceSensor::stopRanging() {
    if (onlineStatus) {
        distanceSensor.stopRanging();
    }
}
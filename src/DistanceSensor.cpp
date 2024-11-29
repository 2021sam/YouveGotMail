#include "DistanceSensor.h"
#include <Wire.h>

unsigned long lastPowerOffTime = 0; // To track time when the power was last turned off
unsigned long resetInterval = 60 * 1000; // 60 seconds


// Constructor with additional parameters for SDA, SCL, and clock speed
DistanceSensor::DistanceSensor(uint8_t address, uint8_t sda, uint8_t scl, uint32_t clockSpeed)
    : onlineStatus(false), i2cAddress(address), sdaPin(sda), sclPin(scl), i2cClockSpeed(clockSpeed) {
    // Optionally initialize any member variables if needed
}


// Initialize the VL53L1X distance sensor and check if it is online
bool DistanceSensor::begin() {
    Wire.begin(sdaPin, sclPin);  // Initialize I2C with specified pins
    Wire.setClock(i2cClockSpeed); // Set the I2C clock speed
    Serial.println("I2C initialized");

    // Print debug information
    Serial.print("SDA: ");
    Serial.println(sdaPin);
    Serial.print("SCL: ");
    Serial.println(sclPin);
    Serial.print("Clock: ");
    Serial.println(i2cClockSpeed);

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


bool DistanceSensor::resetSensor() {
    Serial.println("Attempting to reset the sensor...");
    
    // Try to reinitialize the sensor after waiting for 60 seconds
    lastPowerOffTime = millis(); // Reset the timer
    while (millis() - lastPowerOffTime < resetInterval) {
        yield();
    }

    // After waiting, attempt to reinitialize the sensor
    if (!begin()) {
        Serial.println("Failed to reinitialize the sensor.");
        return false;
    } else {
        Serial.println("Sensor reset successfully.");
        return true;
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

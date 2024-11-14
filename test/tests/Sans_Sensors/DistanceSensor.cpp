#include "DistanceSensor.h"
#include <Wire.h>

// Constructor
DistanceSensor::DistanceSensor() {
    // Optional: Add any initialization if needed
}

// Initialize the VL53L1X distance sensor
bool DistanceSensor::begin() {
    if (distanceSensor.begin() != 0) {
        Serial.println("Distance sensor failed to initialize.");
        return false;  // Return false if initialization fails
    }
    return true;  // Successfully initialized
}

// Get the distance in centimeters
float DistanceSensor::getDistance() {
    distanceSensor.startRanging();  // Start ranging
    while (!distanceSensor.checkForDataReady()) {
        delay(1);  // Wait for data
    }
    int distanceInMM = distanceSensor.getDistance(); // Distance in mm
    distanceSensor.clearInterrupt();  // Clear interrupt
    distanceSensor.stopRanging();  // Stop ranging
    return distanceInMM / 10.0;  // Convert mm to cm
}

// Expose the necessary methods from SFEVL53L1X

void DistanceSensor::startRanging() {
    distanceSensor.startRanging();  // Start ranging
}

bool DistanceSensor::checkForDataReady() {
    return distanceSensor.checkForDataReady(); // Check if data is ready
}

void DistanceSensor::clearInterrupt() {
    distanceSensor.clearInterrupt();  // Clear interrupt
}

void DistanceSensor::stopRanging() {
    distanceSensor.stopRanging();  // Stop ranging
}

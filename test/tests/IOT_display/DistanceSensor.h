#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <SparkFun_VL53L1X.h>

class DistanceSensor {
public:
    // DistanceSensor();  // Constructor
    DistanceSensor(uint8_t address = 0x29);  // Default to the standard address
    bool begin();      // Initialize the sensor
    bool isOnline();   // Check if the sensor is online (initialized and working)
    float getDistance(); // Get the distance in cm

    // Expose the necessary methods from SFEVL53L1X
    // Set a new I²C address
    bool setAddress(uint8_t newAddress);
    void startRanging();  // Start ranging
    bool checkForDataReady(); // Check if data is ready
    void clearInterrupt(); // Clear the interrupt
    void stopRanging();  // Stop ranging

private:
    SFEVL53L1X distanceSensor;  // SparkFun VL53L1X sensor object
    bool onlineStatus;         // Holds the sensor status (online or offline)
    uint8_t i2cAddress;      // Store the I2C address
};

#endif

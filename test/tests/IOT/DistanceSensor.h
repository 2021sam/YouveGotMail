#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <SparkFun_VL53L1X.h>

class DistanceSensor {
public:
    DistanceSensor();  // Constructor
    bool begin();      // Initialize the sensor
    float getDistance(); // Get the distance in cm

    // Expose the necessary methods from SFEVL53L1X
    void startRanging();  // Start ranging
    bool checkForDataReady(); // Check if data is ready
    void clearInterrupt(); // Clear the interrupt
    void stopRanging();  // Stop ranging

private:
    SFEVL53L1X distanceSensor;  // SparkFun VL53L1X sensor object
};

#endif

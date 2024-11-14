#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <BH1750.h>  // Include the BH1750 library

class LightSensor {
public:
    LightSensor();  // Constructor
    bool begin();   // Initialize the sensor
    float getLightLevel(); // Get the light level in lux

private:
    BH1750 lightMeter;  // BH1750 light sensor object
};

#endif

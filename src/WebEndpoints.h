// IOT\WebEndpoints.h

#ifndef WEBENDPOINTS_H
#define WEBENDPOINTS_H
#include "DistanceSensor.h"   // Include your custom DistanceSensor class
#include "LightSensor.h"      // Include your custom LightSensor class
#include "MailService.h"      // Include your custom MailService class

#include <WebServer.h>  // Include the WebServer library
// #include <limits.h>     // For INT_MAX and INT_MIN
// #include <float.h>      // For FLT_MAX and FLT_MIN
#include "GlobalUtils.h"

class WebEndpoints {
public:
// Constructor that takes references to existing sensor objects and a MailService pointer
    // WebEndpoints(WebServer& server, MailService* mailService, 
    //              DistanceSensor& distanceSensor, LightSensor& lightSensor);


    // Constructor that takes references to systemLog and logIndex
    WebEndpoints(WebServer& server, MailService* mailService,
                 String* systemLog, int& logIndex);


    // Method declarations for handling different routes
    void handleRoot();
    void handleDeviceInfo();   // Added this declaration
    void handleDistance();
    void handleLux();
    void handleRSSI();
    void handleEmailAlert();
    void handleSensorData();
    void handleLog();

private:
    WebServer& server;  // Reference to the WebServer object
    String* systemLog;   // Pointer to system log
    int& logIndex;       // Reference to log index

    // DistanceSensor& distanceSensor;
    // LightSensor& lightSensor;
    MailService* mailService; // Use a pointer
    float previousDistance;
    unsigned long lastEmailTime;

    // // Min/Max tracking variables
    // float minLux = FLT_MAX, maxLux = FLT_MIN;
    // float minTof = FLT_MAX, maxTof = FLT_MIN;
    // int minRSSI = INT_MAX, maxRSSI = INT_MIN;

// If you want to avoid adding new #include directives to your project,
//  you can define the initial extreme values for RSSI, lux, and ToF directly
//  using constants or manually set high and low values for initialization.
//    // Min/Max tracking variables initialized to extreme values
//     float minLux = 1e6, maxLux = -1e6;      // Using very large/small initial values
//     float minTof = 1e6, maxTof = -1e6;      // For distance (ToF sensor)
//     int minRSSI = 32767, maxRSSI = -32768;  // Using maximum and minimum 16-bit integer values


    bool isWithinDeliveryWindow();
};

#endif // WEBENDPOINTS_H

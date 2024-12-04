// IOT\WebEndpoints.h

#ifndef WEBENDPOINTS_H
#define WEBENDPOINTS_H
#include "DistanceSensor.h"   // Include your custom DistanceSensor class
#include "LightSensor.h"      // Include your custom LightSensor class
#include "MailService.h"      // Include your custom MailService class

#include <WebServer.h>  // Include the WebServer library
// #include <Preferences.h>      // Include Preferences for persistent storage
#include "GlobalUtils.h"

class WebEndpoints {
public:
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
    void showConfigForm();
    void handleConfig();

private:
    WebServer& server;  // Reference to the WebServer object
    String* systemLog;   // Pointer to system log
    int& logIndex;       // Reference to log index

    MailService* mailService; // Use a pointer
    // Preferences preferences;  // Preferences for managing persistent settings
    float previousDistance;
    unsigned long lastEmailTime;
    bool isWithinDeliveryWindow();
};

#endif // WEBENDPOINTS_H

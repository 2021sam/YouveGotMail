// IOT\WebEndpoints.h

#ifndef WEBENDPOINTS_H
#define WEBENDPOINTS_H
#include "DistanceSensor.h"   // Include your custom DistanceSensor class
#include "LightSensor.h"      // Include your custom LightSensor class
#include "MailService.h"      // Include your custom MailService class

#include <WebServer.h>  // Include the WebServer library

class WebEndpoints {
public:
    // // Constructor that takes a WebServer reference to setup routes
    // // WebEndpoints(WebServer& server);
    //     WebEndpoints(WebServer& server, MailService* mailService); // Updated constructor

// Constructor that takes references to existing sensor objects and a MailService pointer
    WebEndpoints(WebServer& server, MailService* mailService, 
                 DistanceSensor& distanceSensor, LightSensor& lightSensor);


    // Method declarations for handling different routes
    void handleRoot();
    void handleDeviceInfo();   // Added this declaration
    void handleDistance();
    void handleLux();
    void handleRSSI();
    void handleEmailAlert();

private:
    WebServer& server;  // Reference to the WebServer object

    // Add any necessary member variables (e.g., sensors, email service)
     // References to the existing sensor objects
    DistanceSensor& distanceSensor;
    LightSensor& lightSensor;
    // DistanceSensor distanceSensor;
    // LightSensor lightSensor;
    MailService* mailService; // Use a pointer
    float previousDistance;
    unsigned long lastEmailTime;
    bool isWithinDeliveryWindow();
};

#endif // WEBENDPOINTS_H

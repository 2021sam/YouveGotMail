// IOT\IOT.ino
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
// #include "../lib/TFTDisplay.h"  // Include the TFT display class
// #include "../lib/DistanceSensor.h" // Include the new header file
// #include "../lib/LightSensor.h"  // Include your custom LightSensor header
// #include "../lib/WebEndpoints.h"  // Include the WebEndpoints class
// #include "../lib/MailService.h"  // Include the MailService class
// #include "../include/index_html.h"  // Include the HTML header file
// #include "../lib/Alert.h"
// #include "credentials.h"  // Include your credentials for Wi-Fi and email setup
// #include "../lib/WiFiManager.h"  // Include the new WiFiManager header  -- After credentials.h are included.

#include "TFTDisplay.h"  // Include the TFT display class
#include "DistanceSensor.h" // Include the new header file
#include "LightSensor.h"  // Include your custom LightSensor header
#include "WebEndpoints.h"  // Include the WebEndpoints class
#include "MailService.h"  // Include the MailService class
#include "index_html.h"  // Include the HTML header file
#include "Alert.h"
#include "credentials.h"  // Include your credentials for Wi-Fi and email setup
#include "WiFiManager.h"  // Include the new WiFiManager header  -- After credentials.h are included.


#define LED_PIN 21
TFTDisplay display;  // Create a TFTDisplay object


// Define I²C addresses for both sensors
#define DISTANCE_SENSOR_ADDRESS 0x29  // Custom I²C address for the distance sensor
#define LIGHT_SENSOR_ADDRESS 0x23    // I²C address for the light sensor (default 0x23)
// #define LIGHT_SENSOR_ADDRESS 0x5C  // Change to 0x5C if you're using this address


// Instantiate the TFT display, sensors, and mail service
DistanceSensor distanceSensor(DISTANCE_SENSOR_ADDRESS);  // Pass the address to the distance sensor
LightSensor lightSensor(LIGHT_SENSOR_ADDRESS);  // Pass the address to the light sensor

MailService mailService(AUTHOR_EMAIL, AUTHOR_PASSWORD, SMTP_HOST, SMTP_PORT, recipients, NUM_RECIPIENTS);


// Email-related variables
float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0;  // Threshold for distance change to trigger email
const unsigned long firstEmailDelay = 10 * 60 * 1000; // 10 minutes in milliseconds
const unsigned long emailCooldown = 10 * 60 * 1000; // 10 minutes in milliseconds
unsigned long lastEmailTime = 0;

// Delivery window settings
const int deliveryStartHour = 8;  // 8 AM
const int deliveryEndHour = 17;   // 5 PM

WebServer server(80); // Web server listening on port 80
// WebEndpoints endpoints(server, &mailService);       // Create WebEndpoints instance
WebEndpoints endpoints(server, &mailService, distanceSensor, lightSensor);  // Pass sensor objects to WebEndpoints


// Alert alertSystem(distanceSensor, lightSensor, mailService);  // Alert system instance
// Pass the delivery window parameters to the Alert class constructor
Alert alertSystem(distanceSensor, lightSensor, mailService, deliveryStartHour, deliveryEndHour); 


void setup() {
    Serial.begin(115200);
    // pinMode(LED_PIN, OUTPUT);
    // digitalWrite(LED_PIN, HIGH); // Turn off LED at startup

    display.begin();  // Initialize TFT display
    display.showStatusMessage("Initializing...");  // Show a message on TFT to indicate startup
    // delay(2000);  // Give user time to see the message
    
    // Setup Wi-Fi connection
    String ipAddress = setup_WiFi(display);
    display.showStatusMessage(ipAddress);
    delay(2000);

    // Configure time for Pacific Standard Time (UTC-8)
    configTime(-8 * 3600, 0, "pool.ntp.org");

    // Initialize sensors
    Wire.begin();

    // Initialize the distance sensor
    distanceSensor.begin();  // Get the return value of begin()

    // Assuming distanceSensor is initialized and begin() was called earlier
    String statusMessage = "Distance Sensor online status: ";

    // Append "Online" or "Offline" based on the result of distanceSensor.isOnline()
    statusMessage += distanceSensor.isOnline() ? "Online" : "Offline";

    // Now, send this status message to your display and serial monitor
    display.showStatusMessage(statusMessage);  // Display the message on the TFT screen
    Serial.println(statusMessage);  // Output the same status to the serial monitor

    // Initialize the light sensor
    if (!lightSensor.begin()) {
        Serial.println("Light sensor initialization failed!");
        display.showStatusMessage("Light sensor initialization failed!");
    }
    else {
        Serial.println("Distance sensor initialized successfully!");
        // Check if the sensor is online after successful initialization
        Serial.println("Light sensor initialized successfully!");
    }

    // Start the web server
    server.begin();
    Serial.println("WebServer started");

    // Initialize the TFT display
    display.showStatusMessage("Setup Complete!");
    delay(2000);  // Give user time to see the final message
    startTime = millis();
}


void loop() {
    // Handle incoming HTTP requests
    server.handleClient();

    checkWiFiConnection(display); // Check if Wi-Fi is connected, reconnect if necessary

    float currentDistance = distanceSensor.getDistance();
    float lux = lightSensor.getLightLevel();
    int rssi = WiFi.RSSI();
    // controlLED();

    String currentTime = alertSystem.getCurrentTime();  // Get current time via Alert class
    // Print RSSI, distance, and light level on one line
    Serial.printf("[%s] RSSI: %d, Distance: %.2f cm, Light Level: %.2f lux\n", currentTime.c_str(), rssi, currentDistance, lux);


    // Display all data on the TFT screen
    display.showAllData(currentDistance, lux, rssi, currentTime);
    

    if (!distanceSensor.isOnline()) {
            // Serial.println("Distance sensor is offline!");
            // Attempt to reset sensor after a failure
            distanceSensor.resetSensor();
        }



    if (millis() - startTime > firstEmailDelay){
        String statusMessage = alertSystem.checkAndSendEmail(currentDistance, lux);  // Check alert conditions and send email if necessary


        int threshold = 100;
        // alertSystem.monitorLightSensor(threshold);

        if (statusMessage != "") {
        display.showStatusMessage(statusMessage);  // Show the new message on the TFT screen
        delay(10000);
        }
    }
    delay(2000); // Wait before the next measurement
}

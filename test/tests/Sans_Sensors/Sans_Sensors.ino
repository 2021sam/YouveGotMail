// Sans_Sensors.ino

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "TFTDisplay.h"  // Include the TFT display class
#include "WebEndpoints.h"  // Include the WebEndpoints class
#include "DistanceSensor.h" // Include the new header file
#include "LightSensor.h"  // Include your custom LightSensor header
#include "MailService.h"  // Include the MailService class
#include "index_html.h"  // Include the HTML header file
#include "Alert.h"
#include "credentials.h"  // Include your credentials for Wi-Fi and email setup
#include "WiFiManager.h"  // Include the new WiFiManager header  -- After credentials.h are included.
#define LED_PIN 21
TFTDisplay display;  // Create a TFTDisplay object
DistanceSensor distanceSensor; // Instantiate the DistanceSensor class
LightSensor lightSensor;  // Instantiate the LightSensor class
// Instantiate the MailService class with credentials passed to it
MailService mailService(AUTHOR_EMAIL, AUTHOR_PASSWORD, SMTP_HOST, SMTP_PORT, recipients, NUM_RECIPIENTS);


// Email-related variables
float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0;  // Threshold for distance change to trigger email
const unsigned long firstEmailDelay = 5 * 60 * 1000; // 5 minute in milliseconds
const unsigned long emailCooldown = 10000; // 10 seconds in milliseconds
unsigned long lastEmailTime = 0;

// Delivery window settings
const int deliveryStartHour = 8;  // 8 AM
const int deliveryEndHour = 17;   // 5 PM

WebServer server(80); // Web server listening on port 80
WebEndpoints endpoints(server, &mailService);       // Create WebEndpoints instance

// Alert alertSystem(distanceSensor, lightSensor, mailService);  // Alert system instance
// Pass the delivery window parameters to the Alert class constructor
Alert alertSystem(distanceSensor, lightSensor, mailService, deliveryStartHour, deliveryEndHour); 


void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    // digitalWrite(LED_PIN, HIGH); // Turn off LED at startup

    display.begin();  // Initialize TFT display
    display.showStatusMessage("Initializing...");  // Show a message on TFT to indicate startup

    // Setup Wi-Fi connection
    String ipAddress = setup_WiFi(display);
    display.showStatusMessage(ipAddress);
    delay(1000);

    // Configure time for Pacific Standard Time (UTC-8)
    configTime(-8 * 3600, 0, "pool.ntp.org");

    // // Initialize sensors
    // Wire.begin();

    // // Initialize the distance sensor
    // if (!distanceSensor.begin()) {
    //     Serial.println("Sensor initialization failed!");
    //     display.showStatusMessage("Distance sensor initialization failed!");
    //     while (1);  // Halt if sensor fails to initialize
    // }
    // Serial.println("Distance sensor initialized successfully!");

    // // Initialize the light sensor
    // if (!lightSensor.begin()) {
    //     Serial.println("Light sensor initialization failed!");
    //     display.showStatusMessage("Light sensor initialization failed!");
    //     while (1);  // Halt if sensor fails to initialize
    // }
    // Serial.println("Light sensor initialized successfully!");

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

    int rssi = WiFi.RSSI();  // Get the current Wi-Fi RSSI (signal strength)
    String currentTime = alertSystem.getCurrentTime();  // Get current time via Alert class
    Serial.println("Time: " + currentTime + "     RSSI: " + String(rssi) + " dBm");
    // Display current time and Wi-Fi RSSI on the TFT screen
    display.showStatusMessage("Time: " + currentTime + "\nRSSI: " + String(rssi) + " dBm");
    delay(1000); // Wait before the next measurement
}

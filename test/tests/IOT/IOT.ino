// IOT\IOT.ino
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "WiFiManager.h"  // Include the new WiFiManager header
#include "WebEndpoints.h"  // Include the WebEndpoints class
#include "DistanceSensor.h" // Include the new header file
#include "LightSensor.h"  // Include your custom LightSensor header
#include "MailService.h"  // Include the MailService class
#include "index_html.h"  // Include the HTML header file
#include "Alert.h"
#include "credentials.h"  // Include your credentials for Wi-Fi and email setup

#define LED_PIN 21
DistanceSensor distanceSensor; // Instantiate the DistanceSensor class
LightSensor lightSensor;  // Instantiate the LightSensor class
// Instantiate the MailService class with credentials passed to it
MailService mailService(AUTHOR_EMAIL, AUTHOR_PASSWORD, SMTP_HOST, SMTP_PORT, recipients, NUM_RECIPIENTS);


// Email-related variables
float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0;  // Threshold for distance change to trigger email
const unsigned long firstEmailDelay = 60000; // 1 minute in milliseconds
const unsigned long emailCooldown = 10000; // 10 seconds in milliseconds
unsigned long lastEmailTime = 0;

// Delivery window settings
const int deliveryStartHour = 8;  // 8 AM
const int deliveryEndHour = 17;   // 5 PM

WebServer server(80); // Web server listening on port 80
// WebEndpoints endpoints(server); // Instantiate WebEndpoints with the WebServer object

// Create WebEndpoints instance
WebEndpoints endpoints(server, &mailService);

// Alert alertSystem(distanceSensor, lightSensor, mailService);  // Alert system instance
// Pass the delivery window parameters to the Alert class constructor
Alert alertSystem(distanceSensor, lightSensor, mailService, deliveryStartHour, deliveryEndHour); 

// bool isWithinDeliveryWindow()
// {
//     time_t now = time(nullptr);
//     struct tm* timeinfo = localtime(&now);
//     int currentHour = timeinfo->tm_hour;
//     return (currentHour >= deliveryStartHour && currentHour < deliveryEndHour);
// }



void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // Turn off LED at startup

    // Setup Wi-Fi connection
    setup_WiFi();

    // Configure time for Pacific Standard Time (UTC-8)
    configTime(-8 * 3600, 0, "pool.ntp.org");

    // Initialize sensors
    Wire.begin();

    // Initialize the distance sensor
    if (!distanceSensor.begin()) {
        Serial.println("Sensor initialization failed!");
        while (1);  // Halt if sensor fails to initialize
    }
    Serial.println("Distance sensor initialized successfully!");

    // Initialize the light sensor
    if (!lightSensor.begin()) {
        Serial.println("Light sensor initialization failed!");
        while (1);  // Halt if sensor fails to initialize
    }
    Serial.println("Light sensor initialized successfully!");

    // Start the web server
    server.begin();
    Serial.println("WebServer started");
}


void loop() {
    // Handle incoming HTTP requests
    server.handleClient();

    checkWiFiConnection(); // Check if Wi-Fi is connected, reconnect if necessary

    float currentDistance = distanceSensor.getDistance();
    float lux = lightSensor.getLightLevel();
    int rssi = WiFi.RSSI();
    // controlLED();

    String currentTime = alertSystem.getCurrentTime();  // Get current time via Alert class
    Serial.print("Current time in SF Bay Area: ");
    Serial.println(currentTime);  // Print time via Serial Monitor

    // Print RSSI, distance, and light level on one line
    Serial.printf("RSSI: %d, Distance: %.2f cm, Light Level: %.2f lux\n", rssi, currentDistance, lux);

    alertSystem.checkAndSendEmail();  // Check alert conditions and send email if necessary

    delay(1000); // Wait before the next measurement
}

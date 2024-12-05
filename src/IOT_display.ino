// IOT\IOT.ino
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "TFTDisplay.h"  // Include the TFT display class
#include "DistanceSensor.h" // Include the new header file
#include "LightSensor.h"  // Include your custom LightSensor header
#include "WebEndpoints.h"  // Include the WebEndpoints class
#include "index_html.h"  // Include the HTML header file
#include "Alert.h"
#include "WiFiMaintenance.h"  // Include the new WiFiManager header  -- After credentials.h are included.
#include "GlobalUtils.h"
#include <WiFiManager.h> // Include WiFiManager
#include "ButtonHandler.h" // Include the new header file for button handling

#define LED_PIN 21
TFTDisplay display;  // Create a TFTDisplay object

// Define I²C addresses for both sensors
#define DISTANCE_SENSOR_ADDRESS 0x29  // Custom I²C address for the distance sensor
#define LIGHT_SENSOR_ADDRESS 0x23    // I²C address for the light sensor (default 0x23)


// Instantiate the TFT display, sensors, and mail service
DistanceSensor distanceSensor(DISTANCE_SENSOR_ADDRESS);  // Pass the address to the distance sensor
LightSensor lightSensor(LIGHT_SENSOR_ADDRESS);  // Pass the address to the light sensor

// Email-related variables
unsigned long startTime;
const float distanceThresholdCM = 10.0;  // Threshold for distance change to trigger email
unsigned long lastMeasurementTime = 0;  // Variable to track last measurement time

// Delivery window settings
const int deliveryStartHour = 8;  // 8 AM
const int deliveryEndHour = 17;   // 5 PM

WebServer server(80); // Web server listening on port 80
WebEndpoints endpoints(server, systemLog, logIndex);   

// Pointer for Alert class
Alert* alert = nullptr;

// volatile bool triggerAPMode = false;  // Flag to trigger AP mode
extern volatile bool triggerAPMode;

void setup() {
    Serial.begin(115200);
    // pinMode(LED_PIN, OUTPUT);
    // digitalWrite(LED_PIN, HIGH); // Turn off LED at startup

    display.begin();  // Initialize TFT display
    display.showStatusMessage("Initializing...");  // Show a message on TFT to indicate startup

    // Configure time for Pacific Standard Time (UTC-8)
    configTime(-8 * 3600, 0, "pool.ntp.org");       //  This requires a small delay to configure time.

      // Initialize the globalMailService
    initializeGlobalMailService();

    // Setup Wi-Fi connection
    String ipAddress = setup_WiFi(display);
    display.showStatusMessage(ipAddress);
    delay(2000);

    attachInterrupt(BUTTON_PIN, handleButtonPress, CHANGE); // Trigger on both press and release
    // Initialize Wi-Fi and button functionality
    setupWiFiAndButton();

    // Initialize sensors
    Wire.begin();

    // Initialize the distance sensor
    distanceSensor.begin();  // Get the return value of begin()
    String statusMessage = "Distance Sensor online status: ";
    statusMessage += distanceSensor.isOnline() ? "Online" : "Offline";
    display.showStatusMessage(statusMessage);  // Display the message on the TFT screen
    Serial.println(statusMessage);  // Output the same status to the serial monitor

    // Start the web server
    server.begin();
    Serial.println("WebServer started");

    previousTof = distanceSensor.getDistance();
    // alert = new Alert(*mailService, deliveryStartHour, deliveryEndHour);

    // Initialize Alert with the dereferenced MailService pointer
    alert = new Alert(deliveryStartHour, deliveryEndHour);

    // addToLog("Device IP Address: " + ipAddress);
    addToLog("Initial Distance: " + String(previousTof));

    // Initialize light sensor
    lightSensor.begin();
    float initialLux = lightSensor.getLightLevel();
    String luxMessage = "Initial Lux: " + String(initialLux);
    addToLog(luxMessage);  // Add the lux reading to the system log
    display.showStatusMessage(luxMessage);  // Display lux on the screen
    delay(2000);

    // Initialize the TFT display
    display.showStatusMessage("Setup Complete!");
    delay(2000);  // Give user time to see the final message
    startTime = millis();
    addToLog("Online startTime");
}


void loop() {
    // Handle incoming HTTP requests
    server.handleClient();
    
    if (triggerAPMode) {
        triggerAPMode = false;  // Reset the flag after calling startAPMode
        deleteWiFiCredentials();
        display.showStatusMessage("Credentials Erased... Restarting...");
        delay(2000);
        ESP.restart();
    }

    unsigned long currentMillis = millis();  // Get current time
    if (currentMillis - lastMeasurementTime > 1000) {
        lastMeasurementTime = currentMillis;  // Update the last measurement time
        scheduledTasks();
    }
}

void scheduledTasks(){
    float lux, currentDistance;
    String currentTime = getCurrentTime();  // Get current time via Alert class
    int rssi = WiFi.RSSI();

    if (distanceSensor.isOnline()) {
        lux = lightSensor.getLightLevel();
        currentDistance = distanceSensor.getDistance();
        updateSensorValues(rssi, lux, currentDistance);
        // alert->checkAndSendEmail();  // Check alert conditions and send email if necessary
    }
    else {
            lux = -1;
            currentDistance = -1;
            updateSensorValues(rssi, -1, -1);
            bool resurrectTOF = distanceSensor.resetSensor();
            if (resurrectTOF)
                addToLog("Distance sensor is resurrected & back online.");
    }
    display.showAllData(currentDistance, lux, rssi, currentTime);
    Serial.printf("[%s] RSSI: %d, Distance: %.2f cm, Light Level: %.2f lux\n", currentTime.c_str(), rssi, currentDistance, lux);
}

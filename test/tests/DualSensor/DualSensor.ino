/*
 * You've Got Mail
 * 
 * This program uses an ESP32 microcontroller with a VL53L1X distance sensor
 * and a BH1750 light sensor to detect mail delivery.
 * It sends an email alert whenever a significant change in distance is detected.
 * 
 * Author: Sam Portillo
 * Location: SF Bay Area, California
 * Date: October 30, 2024
 * 
 * Libraries Required:
 * - SparkFun VL53L1X
 * - BH1750 (https://github.com/claws/BH1750)
 * - ESP Mail Client
 */

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#include <BH1750.h> // Include the BH1750 library
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "credentials.h" // Include your credentials file

// Define the LED pin
#define LED_PIN 21

// Create an instance of the VL53L1X sensor
SFEVL53L1X distanceSensor;
// Create an instance of the BH1750 sensor
BH1750 lightMeter;

// Declare the global used SMTPSession object for SMTP transport
SMTPSession smtp;

// Declare the Session_Config for user-defined session credentials
Session_Config config;

// Variables for distance measurement
float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0; // Change threshold in cm
const unsigned long firstEmailDelay = 60000; // 1 minute in milliseconds
const unsigned long emailCooldown = 10000; // 10 seconds in milliseconds
unsigned long lastEmailTime = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT); // Set LED pin as output

    // Initialize Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    // Initialize the I2C bus
    Wire.begin();

    // Initialize the VL53L1X sensor
    if (distanceSensor.begin() != 0) { // Begin returns 0 on a good init
        Serial.println("Distance sensor failed to begin. Please check wiring. Freezing...");
        while (1);
    }
    Serial.println("Distance sensor online!");

    // Initialize the BH1750 sensor
    lightMeter.begin();
    Serial.println("BH1750 light sensor online!");

    // Set up SMTP configuration
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    config.login.user_domain = F("127.0.0.1");

    // Enable debugging
    smtp.debug(1);

    // Record the start time
    startTime = millis();
}

void loop() {
    // Read distance measurement
    float currentDistance = getDistance();
    Serial.printf("Distance: %.2f cm\n", currentDistance);

    // Read light level from BH1750
    float lux = lightMeter.readLightLevel();
    Serial.printf("Light Level (lux): %.2f\n", lux);

    // Control the LED based on Wi-Fi signal strength
    controlLED();

    // Check if enough time has passed and the change in distance exceeds the threshold
    if (millis() - startTime > firstEmailDelay) {
        if (abs(currentDistance - previousDistance) >= distanceThresholdCM && (millis() - lastEmailTime > emailCooldown)) {
            sendEmail(currentDistance, lux);
            previousDistance = currentDistance; // Update the previous distance
            lastEmailTime = millis(); // Record the time of the last email
        } else {
            previousDistance = currentDistance;
        }
    } else {
        previousDistance = currentDistance; // Initialize previousDistance after 1st reading
    }

    delay(1000); // Wait before the next measurement
}

void controlLED() {
    int rssi = WiFi.RSSI(); // Get the RSSI value
    Serial.printf("RSSI: %d dBm\n", rssi);

    if (WiFi.status() == WL_CONNECTED) {
        // Strong signal
        if (rssi > -60) {
            digitalWrite(LED_PIN, HIGH); // Turn on LED solidly
            delay(500); // Keep it on for half a second
            digitalWrite(LED_PIN, LOW);
            delay(500); // Off for half a second
        }
        // Moderate signal
        else if (rssi > -70) {
            digitalWrite(LED_PIN, HIGH); // Turn on LED solidly
            delay(200); // Keep it on for a shorter time
            digitalWrite(LED_PIN, LOW);
            delay(200); // Off for a shorter time
        }
        // Weak signal
        else {
            digitalWrite(LED_PIN, HIGH); // Blink slowly
            delay(1000);
            digitalWrite(LED_PIN, LOW);
            delay(1000);
        }
    } else {
        // No Wi-Fi connection
        digitalWrite(LED_PIN, LOW); // Turn off the LED
    }
}

float getDistance() {
    distanceSensor.startRanging(); // Initiate measurement
    while (!distanceSensor.checkForDataReady()) {
        delay(1);
    }
    int distanceInMM = distanceSensor.getDistance(); // Get the result of the measurement in mm
    distanceSensor.clearInterrupt();
    distanceSensor.stopRanging();

    return distanceInMM / 10.0; // Convert to cm and return
}

void sendEmail(float distance, float lux) {
    // Set up the email message
    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = F("You've Got Mail!");

    // Add recipients from the list defined in credentials.h
    for (int i = 0; i < NUM_RECIPIENTS; i++) {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

    // Print the number of recipients
    Serial.printf("Number of recipients: %d\n", NUM_RECIPIENTS);

    String htmlMsg = "<p>The distance has changed significantly!</p>"
                     "<p>Previous distance: " + String(previousDistance) + " cm</p>"
                     "<p>Current distance: " + String(distance) + " cm</p>"
                     "<p>Light Level (lux): " + String(lux) + "</p>";
    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");

    // Connect and send the email
    if (!smtp.connect(&config)) {
        Serial.printf("Connection error: %s\n", smtp.errorReason().c_str());
        return;
    }

    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
    } else {
        Serial.println("Email sent successfully!");
    }
}

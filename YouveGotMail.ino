/*
 * You've Got Mail
 * 
 * This program uses an ESP32 microcontroller and a VL53L1X distance sensor
 * to detect when mail is delivered to your mailbox. It sends an email alert
 * whenever a significant change in distance is detected.
 * 
 * Features:
 * - Monitors mailbox door opening and or mail being placed in or removed from mailbox.
 * - Sends email alerts upon detecting mail.
 * - Configurable distance thresholds.
 * 
 * Setup Instructions:
 * 1. Connect the VL53L1X sensor to the ESP32 --  Seeed Studio with U.FL
 * 2. Update the Wi-Fi and email credentials in `credentials.h`.
 * 3. Upload the code using Arduino IDE.
 * 
 * Author: Sam Portillo
 * Location: SF Bay Area, California
 * Date: October 30, 2024
 */


#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "credentials.h" // Include your credentials file

// Define the LED pin (GPIO 21 for built-in LED)
#define LED_PIN 21

// Create an instance of the VL53L1X sensor
SFEVL53L1X distanceSensor;

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
  pinMode(LED_PIN, OUTPUT);

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

  // Initialize the VL53L1X sensor
  Wire.begin();
  if (distanceSensor.begin() != 0) { // Begin returns 0 on a good init
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1);
  }
  Serial.println("Sensor online!");

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

  // LED brightness based on distance (optional)
  int ledBrightness = map(currentDistance, 0, 400, 255, 0); // Adjust range as needed
  ledBrightness = constrain(ledBrightness, 0, 255);
  ledcWrite(LED_PIN, ledBrightness); // Use ledcWrite for ESP32

  // Check if enough time has passed and the change in distance exceeds the threshold
  if (millis() - startTime > firstEmailDelay) {
    if (abs(currentDistance - previousDistance) >= distanceThresholdCM && (millis() - lastEmailTime > emailCooldown)) {
      sendEmail(currentDistance);
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

#include "credentials.h" // Include your credentials file

void sendEmail(float distance) {
    // Set up the email message
    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = F("Distance Measurement Alert");

    // Add recipients from the list defined in credentials.h
    for (int i = 0; i < NUM_RECIPIENTS; i++) {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

    // Print the number of recipients
    Serial.printf("Number of recipients: %d\n", NUM_RECIPIENTS);

    String htmlMsg = "<p>The distance has changed significantly!</p><p>Previous distance: " + String(previousDistance) + " cm</p><p>Current distance: " + String(distance) + " cm</p>";
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

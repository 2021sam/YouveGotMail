#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#include <BH1750.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h> // For email functionality
#include "credentials.h" // Include your credentials for Wi-Fi and email setup

#define LED_PIN 21

SFEVL53L1X distanceSensor;
BH1750 lightMeter;
SMTPSession smtp;
Session_Config config;

float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0;
const unsigned long firstEmailDelay = 60000; // 1 minute in milliseconds
const unsigned long emailCooldown = 10000; // 10 seconds in milliseconds
unsigned long lastEmailTime = 0;

// Delivery window settings
const int deliveryStartHour = 8;  // 8 AM
const int deliveryEndHour = 17;   // 5 PM

// Wi-Fi setup function using credentials from credentials.h
void setup_WiFi()
{
    Serial.println();
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID); // From credentials.h

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Use Wi-Fi credentials from credentials.h

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);  // Wait for 5 seconds before trying again
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// Check Wi-Fi connection status and reconnect if necessary
void checkWiFiConnection()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wi-Fi connection lost. Reconnecting...");
        setup_WiFi(); // Reconnect Wi-Fi
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // Turn off LED at startup

    // Setup Wi-Fi connection
    setup_WiFi();

    // Configure time for Pacific Standard Time (UTC-8)
    configTime(-8 * 3600, 0, "pool.ntp.org");

    Wire.begin();
    if (distanceSensor.begin() != 0)
    {
        Serial.println("Distance sensor failed to begin.");
        while (1);
    }
    lightMeter.begin();
    Serial.println("Sensors online!");

    // Set up SMTP configuration
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    config.login.user_domain = F("127.0.0.1");

    smtp.debug(1); // Enable debugging
    startTime = millis();

    // Print initial delivery window status
    bool inDeliveryWindow = isWithinDeliveryWindow();
    if (inDeliveryWindow)
    {
        Serial.println("High alert: Currently within the delivery window.");
    }
    else
    {
        Serial.println("High alert: Outside of the delivery window.");
    }
}

void loop()
{
    checkWiFiConnection(); // Check if Wi-Fi is connected, reconnect if necessary

    float currentDistance = getDistance();
    float lux = lightMeter.readLightLevel();
    int rssi = WiFi.RSSI();
    controlLED();

    // Print the current time
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.print("Current time in SF Bay Area: ");
    Serial.println(buffer);
    
    // Print RSSI, distance, and light level on one line
    Serial.printf("RSSI: %d, Distance: %.2f cm, Light Level: %.2f lux\n", rssi, currentDistance, lux);
    
    // Delivery window check
    bool inDeliveryWindow = isWithinDeliveryWindow();

    // Check if enough time has passed and the change in distance exceeds the threshold
    if (millis() - startTime > firstEmailDelay)
    {
        if (abs(currentDistance - previousDistance) >= distanceThresholdCM && (millis() - lastEmailTime > emailCooldown))
        {
            if (inDeliveryWindow)
            {
                // Immediate alert during delivery window
                sendEmail(currentDistance, lux, true); // Include delivery window status
                previousDistance = currentDistance; // Update previous distance
                lastEmailTime = millis(); // Record the time of the last email
            }
            else
            {
                // Outside delivery window, confirm the alert
                blinkLED(1000); // Blink LED for confirmation
                float confirmedDistance = getDistance();
                Serial.printf("Confirmed Distance: %.2f cm\n", confirmedDistance);
                if (abs(confirmedDistance - previousDistance) >= distanceThresholdCM)
                {
                    sendEmail(confirmedDistance, lux, false); // Include delivery window status
                    previousDistance = confirmedDistance;
                    lastEmailTime = millis();
                }
                else
                {
                    Serial.println("Alert canceled, distance is now safe.");
                }
            }
        }
        else
        {
            previousDistance = currentDistance; // Update previous distance for the next loop
        }
    }
    else
    {
        previousDistance = currentDistance; // Initialize previousDistance after 1st reading
    }

    delay(1000); // Wait before the next measurement
}

bool isWithinDeliveryWindow()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    return (currentHour >= deliveryStartHour && currentHour < deliveryEndHour);
}

float getDistance()
{
    distanceSensor.startRanging();
    while (!distanceSensor.checkForDataReady())
    {
        delay(1);
    }
    int distanceInMM = distanceSensor.getDistance();
    distanceSensor.clearInterrupt();
    distanceSensor.stopRanging();
    return distanceInMM / 10.0; // Convert to cm
}

void sendEmail(float distance, float lux, bool inDeliveryWindow)
{
    // Set up the email message
    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = F("You've Got Mail!");

    // Add recipients from the list defined in credentials.h
    for (int i = 0; i < NUM_RECIPIENTS; i++)
    {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

    // Create the HTML message
    String htmlMsg = "<p>Previous distance: " + String(previousDistance) + " cm</p>"
                     "<p>Current distance: " + String(distance) + " cm</p>"
                     "<p>Lux: " + String(lux) + "</p>"
                     "<p>RSSI: " + String(WiFi.RSSI()) + " dBm</p>" // Include RSSI
                     "<p>Delivery Window Status: " + String(inDeliveryWindow ? "High" : "Low") + " Alert.</p>";
    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");

    // Connect and send the email
    if (!smtp.connect(&config))
    {
        Serial.printf("Connection error: %s\n", smtp.errorReason().c_str());
        return;
    }

    if (!MailClient.sendMail(&smtp, &message))
    {
        Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
    }
    else
    {
        Serial.println("Email sent successfully!");
    }
}

void controlLED()
{
    // Control logic for LED removed since it's handled during blinking
}

void blinkLED(unsigned long duration)
{
    unsigned long start = millis();
    while (millis() - start < duration)
    {
        digitalWrite(LED_PIN, LOW);  // Turn on LED (active low)
        delay(250);
        digitalWrite(LED_PIN, HIGH); // Turn off LED
        delay(250);
    }
}

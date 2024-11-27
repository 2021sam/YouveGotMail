#ifndef ALERT_H
#define ALERT_H

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "DistanceSensor.h"
#include "LightSensor.h"
#include "MailService.h"
#include "GlobalUtils.h"

#define LED_PIN 21

// Alert class to handle distance checks and email notifications
class Alert {
private:
    MailService& mailService;        // Reference to the mail service
    float previousDistance = 0;      // Previous distance measurement
    // unsigned long lastEmailTime = 0; // Timestamp of the last email sent
    unsigned long startDelayTime = 0; // Timestamp of the last email sent
    const int distanceThresholdCM = 10;  // Minimum distance change for alert (in cm)
    const unsigned long emailCooldown = 60 * 1000; // Cooldown time between email alerts (in ms)
    int deliveryStartHour;
    int deliveryEndHour;
    String currentState = "closed";
    String previousState = "closed";


public:
    Alert(MailService& mailService, int deliveryStartHour, int deliveryEndHour);

    // Method to check the conditions and send an email if necessary
    String checkAndSendEmail(int currentDistance, int lux);

    // Method to check if the current time is within the delivery window (8 AM to 5 PM)
    bool isWithinDeliveryWindow();

    // Method to blink an LED for a confirmation (e.g., during confirmation phase)
    void blinkLED(int delayTime);

    int monitorLightSensor(int threshold);
};

#endif  // ALERT_H

#ifndef ALERT_H
#define ALERT_H

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "DistanceSensor.h"
#include "LightSensor.h"
#include "GlobalUtils.h"

#define LED_PIN 21

// Alert class to handle distance checks and email notifications
class Alert {
private:
    unsigned long startDelayTime = 0; // Timestamp of the last email sent
    const int distanceThresholdCM = 10;  // Minimum distance change for alert (in cm)
    const unsigned long emailCooldown = 60 * 1000; // Cooldown time between email alerts (in ms)
    int deliveryStartHour;
    int deliveryEndHour;
    String currentState = "closed";
    String previousState = "closed";


public:
    Alert(int deliveryStartHour, int deliveryEndHour);

    // Method to check the conditions and send an email if necessary
    void checkAndSendEmail();

    // Method to check if the current time is within the delivery window (8 AM to 5 PM)
    bool isWithinDeliveryWindow();

    // Method to blink an LED for a confirmation (e.g., during confirmation phase)
    void blinkLED(int delayTime);
};

#endif  // ALERT_H

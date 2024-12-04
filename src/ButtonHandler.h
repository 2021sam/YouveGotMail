#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <WiFiManager.h>

// Pin for the button and other constants
const uint8_t BUTTON_PIN = 0;           // Button GPIO pin
const uint32_t PRESS_DURATION = 3000;    // Required press duration in milliseconds

// Declare the functions
void buttonTask(void *parameter);
void handleButtonPress();
void setupWiFiAndButton();

extern void startAPMode(); // Declare external function for WiFi reset logic

#endif

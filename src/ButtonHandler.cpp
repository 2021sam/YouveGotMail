#include "ButtonHandler.h"

WiFiManager wm; // WiFiManager instance

// Button state tracking variables
volatile bool buttonPressed = false;    // Flag for button press
volatile unsigned long pressStartTime = 0; // Time when the button was first pressed
volatile bool isButtonHeld = false;     // Tracks if the button is being held

// Interrupt Service Routine (ISR) for the button
void IRAM_ATTR handleButtonPress() {
    if (digitalRead(BUTTON_PIN) == LOW) { // Button pressed
        if (!isButtonHeld) {
            pressStartTime = millis();   // Record the start time
            isButtonHeld = true;
        }
    } else { // Button released
        isButtonHeld = false;
        buttonPressed = false; // Reset the pressed flag on release
    }
}

// Task to handle button press with duration check
void buttonTask(void *parameter) {
    while (true) {
        if (isButtonHeld) {
            unsigned long currentTime = millis();
            if (!buttonPressed && (currentTime - pressStartTime >= PRESS_DURATION)) {
                buttonPressed = true; // Valid 3-second press detected
            }
        }

        if (buttonPressed) {
            Serial.println("Button held for 3 seconds! Resetting WiFi settings...");
            wm.resetSettings(); // Reset WiFiManager settings



            // Start WiFiManager config portal for reconfiguration
            Serial.println("Starting WiFi Config Portal...");
            wm.startConfigPortal("ESP_Reconfigure", "reconfigure123");

            // Restart ESP after configuration to apply new settings
            Serial.println("Rebooting ESP...");
            ESP.restart();



            buttonPressed = false; // Reset the flag after processing
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Yield to other tasks
    }
}

// Setup WiFi and button functionality
void setupWiFiAndButton() {
    // Button setup
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN, handleButtonPress, CHANGE); // Attach ISR for button press

    // Create button task
    xTaskCreate(
        buttonTask,        // Task function
        "Button Task",     // Task name
        1000,              // Stack size (bytes)
        NULL,              // Parameter to pass
        1,                 // Task priority
        NULL               // Task handle
    );

    // WiFiManager setup
    Serial.println("Setting up WiFiManager...");

    // Connect to WiFi
    bool res = wm.autoConnect("AutoConnectAP", "password"); // password-protected AP
    
    if (!res) {
        Serial.println("Failed to connect to WiFi. Restarting...");
        ESP.restart();
    } else {
        Serial.println("Connected to WiFi!");
    }
}

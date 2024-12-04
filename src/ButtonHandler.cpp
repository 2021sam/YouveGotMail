#include "ButtonHandler.h"

// Button state tracking variables
volatile bool buttonPressed = false;    // Flag for button press
volatile unsigned long pressStartTime = 0; // Time when the button was first pressed
volatile bool isButtonHeld = false;     // Tracks if the button is being held
volatile bool triggerAPMode = false;  // Flag to trigger AP mode


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
        Serial.println("Button released");
        unsigned long currentTime = millis();
        if (!buttonPressed && (currentTime - pressStartTime >= PRESS_DURATION)) {
            Serial.println("Button held for 3+ seconds! Resetting WiFi settings...");
            buttonPressed = true; // Valid 3-second press detected
            triggerAPMode = true;  // Set flag to trigger AP mode
        }
    }
}

// Task to handle button press with duration check
void buttonTask(void *parameter) {
    // Serial.println("b");
    while (true) {
        // Serial.println("c");
        if (isButtonHeld) {
        }

        if (buttonPressed) {
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
        4096,              // Stack size (bytes)
        NULL,              // Parameter to pass
        1,                 // Task priority
        NULL               // Task handle
    );
}

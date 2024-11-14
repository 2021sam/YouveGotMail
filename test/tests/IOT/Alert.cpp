#include "Alert.h"

// Constructor: Initializes the alert system with references to the sensors and mail service
// Alert::Alert(DistanceSensor& distSensor, LightSensor& lightSensor, MailService& mailService)
//     : distanceSensor(distSensor), lightSensor(lightSensor), mailService(mailService) {
//     previousDistance = distanceSensor.getDistance();  // Initialize previous distance with the current reading
// }



Alert::Alert(DistanceSensor& distanceSensor, LightSensor& lightSensor, MailService& mailService,
             int deliveryStartHour, int deliveryEndHour)
    : distanceSensor(distanceSensor), lightSensor(lightSensor), mailService(mailService),
      deliveryStartHour(deliveryStartHour), deliveryEndHour(deliveryEndHour) {}



// Function to return the current time as a string
String Alert::getCurrentTime() {
    // Get the current time from the system clock
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    // Format the time into a readable string
    char buffer[30];  // Buffer size of 30 is enough for formatted time string
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Return the formatted time as a String
    return String(buffer);
}


// Check if the current time is within the delivery window (8 AM to 5 PM)
bool Alert::isWithinDeliveryWindow() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    return (currentHour >= deliveryStartHour && currentHour < deliveryEndHour);
}

// Method to blink an LED for confirmation (e.g., during confirmation phase)
void Alert::blinkLED(int delayTime) {
    digitalWrite(LED_PIN, HIGH);  // Turn LED on
    delay(delayTime);
    digitalWrite(LED_PIN, LOW);   // Turn LED off
    delay(delayTime);
}

// Check the conditions and send an email if necessary
void Alert::checkAndSendEmail() {
    float currentDistance = distanceSensor.getDistance();
    float lux = lightSensor.getLightLevel();
    bool inDeliveryWindow = isWithinDeliveryWindow();

    // Check if enough time has passed and the distance has changed significantly
    if (millis() - lastEmailTime > emailCooldown) {
        if (abs(currentDistance - previousDistance) >= distanceThresholdCM) {
            if (inDeliveryWindow) {
                // Send email during the delivery window
                mailService.sendEmail(previousDistance, currentDistance, lux, true);
                Serial.println("Email sent: Delivery window active.");
            } else {
                // Outside the delivery window, confirm the alert
                blinkLED(1000); // Blink LED for confirmation
                float confirmedDistance = distanceSensor.getDistance();
                Serial.printf("Confirmed Distance: %.2f cm\n", confirmedDistance);

                if (abs(confirmedDistance - previousDistance) >= distanceThresholdCM) {
                    mailService.sendEmail(previousDistance, confirmedDistance, lux, false);
                    Serial.println("Email sent: Outside delivery window, alert confirmed.");
                } else {
                    Serial.println("Alert canceled, distance is now safe.");
                }
            }

            // Update previous distance and last email time
            previousDistance = currentDistance;
            lastEmailTime = millis();
        } else {
            // If the change in distance isn't enough, update the previous distance
            previousDistance = currentDistance;
        }
    }
}

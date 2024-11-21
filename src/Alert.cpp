#include "Alert.h"

Alert::Alert(DistanceSensor& distanceSensor, LightSensor& lightSensor, MailService& mailService,
             int deliveryStartHour, int deliveryEndHour)
    : distanceSensor(distanceSensor), lightSensor(lightSensor), mailService(mailService),
      deliveryStartHour(deliveryStartHour), deliveryEndHour(deliveryEndHour) {
        lastEmailTime = millis();
        // previousDistance = distanceSensor.getDistance();
      }

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
String Alert::checkAndSendEmail(float currentDistance, float lux) {
    // float currentDistance = distanceSensor.getDistance();
    // float lux = lightSensor.getLightLevel();
    bool inDeliveryWindow = isWithinDeliveryWindow();
    String statusMessage = "";  // Variable to store the status message

    // Check if enough time has passed and the distance has changed significantly
    if (millis() - lastEmailTime > emailCooldown) {
        if (abs(currentDistance - previousDistance) >= distanceThresholdCM) {
            if (inDeliveryWindow) {
                // Send email during the delivery window
                mailService.sendEmail(previousDistance, currentDistance, lux, true);
                Serial.println("Email sent: Delivery window active.");
                statusMessage += "Email sent: Delivery window active.\n";
            } else {
                // Outside the delivery window, confirm the alert
                // blinkLED(1000); // Blink LED for confirmation
                Serial.printf("I will confirm the Previous Distance: %.2f cm\n", previousDistance);
                delay(2000);
                float confirmedDistance = distanceSensor.getDistance();
                Serial.printf("Confirmed Distance: %.2f cm\n", confirmedDistance);
                // statusMessage += "Confirmed Distance: " + String(confirmedDistance) + " cm\n";

                if (abs(confirmedDistance - previousDistance) >= distanceThresholdCM) {
                      // Get the current time as a string
                    mailService.sendEmail(previousDistance, confirmedDistance, lux, false);
                    Serial.println("Email sent: Outside delivery window, alert confirmed.");
                    statusMessage += "Email sent: Outside delivery window, alert confirmed.\n";
                } else {
                    Serial.println("Alert canceled, distance is now safe.");
                    statusMessage += "Alert canceled, distance is now safe.\n";
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
    return statusMessage;  // Return the status message
}


int Alert::monitorLightSensor(int threshold) {
    // Trigger the light sensor and get the reading
    int lux = lightSensor.getLightLevel();

    // Check the light level against the threshold
    if (lux >= threshold) {
            Serial.println("Light level exceeds threshold. Mail detected!");
            float currentDistance = distanceSensor.getDistance();
            mailService.sendEmail(previousDistance, currentDistance, lux, true);
    }
}

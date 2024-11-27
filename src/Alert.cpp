#include "Alert.h"

Alert::Alert(MailService& mailService, int deliveryStartHour, int deliveryEndHour)
    : mailService(mailService),
      deliveryStartHour(deliveryStartHour), deliveryEndHour(deliveryEndHour) {
        startDelayTime = millis();
      }


void Alert::setPreviousDistance(float distance) {
    previousDistance = distance;
    Serial.printf("Previous distance manually set to: %.2f cm\n", previousDistance);
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


String Alert::checkAndSendEmail(int currentDistance, int lux) {
    String statusMessage = "";  // Status message to return

    if (mail_alert)
    {
        mail_alert = false;
        mailService.sendEmail(previousTof, currentDistance, lux, true);
        statusMessage = "Email sent: Mailbox is open.";
        Serial.println(statusMessage);
        addToLog(statusMessage);
    }

    return statusMessage;  // Return the status for display or debugging
}



int Alert::monitorLightSensor(int threshold) {
    // Trigger the light sensor and get the reading
    // int lux = lightSensor.getLightLevel();

    // Check the light level against the threshold
    // if (lux >= threshold) {
    //         Serial.println("Light level exceeds threshold. Mail detected!");
    //         // float currentDistance = distanceSensor.getDistance();
    //         mailService.sendEmail(previousDistance, currentDistance, lux, true);
    // }
    return 0;
}

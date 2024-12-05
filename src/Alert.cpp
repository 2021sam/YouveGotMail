#include "Alert.h"

Alert::Alert(MailService &mailService, int deliveryStartHour, int deliveryEndHour)
    : mailService(mailService),
      deliveryStartHour(deliveryStartHour), deliveryEndHour(deliveryEndHour)
{
    startDelayTime = millis();
}

// Check if the current time is within the delivery window (8 AM to 5 PM)
bool Alert::isWithinDeliveryWindow()
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    return (currentHour >= deliveryStartHour && currentHour < deliveryEndHour);
}

// Method to blink an LED for confirmation (e.g., during confirmation phase)
void Alert::blinkLED(int delayTime)
{
    digitalWrite(LED_PIN, HIGH); // Turn LED on
    delay(delayTime);
    digitalWrite(LED_PIN, LOW); // Turn LED off
    delay(delayTime);
}

void Alert::checkAndSendEmail()
{
    if (mail_alert)
    {
        mail_alert = false;
        if (globalMailService == nullptr)
        {
            Serial.println("globalMailService is not initialized!");
            return;
        }

     // Create the HTML message
        String htmlMsg = "<p>Time: " + getCurrentTime() + "</p>" // Include the current time
                        "<p>Previous distance: " + String(previousTof) + " cm</p>"
                        "<p>Current distance: " + String(currentTof) + " cm</p>"
                        "<p>Lux: " + String(currentLux) + "</p>"
                        "<p>RSSI: " + String(currentRSSI) + " dBm</p>"
                        "<p>Delivery Window Status: " + String(isWithinDeliveryWindow() ? "High" : "Low") + " Alert.</p>";

        // Attempt to send the email
        if (!globalMailService->sendEmail(htmlMsg))
        {
            String errorMessage = "Failed to send email: Check your SMTP configuration.";
            Serial.println(errorMessage);
            addToLog(errorMessage);
            return;
        }

        String statusMessage = "Email sent: Mailbox is open.";
        Serial.println(statusMessage);
        addToLog(statusMessage);
    }
}

#include "MailService.h"

MailService::MailService(const char* senderEmail, const char* senderPassword, const char* smtpHost, int smtpPort, 
                         const char* recipients[], int numRecipients) {
    this->senderEmail = senderEmail;
    this->senderPassword = senderPassword;
    this->smtpHost = smtpHost;
    this->smtpPort = smtpPort;
    this->recipients = recipients;
    this->numRecipients = numRecipients;

    // Initialize the session configuration
    config.login.email = senderEmail;
    config.login.password = senderPassword;
    config.server.host_name = smtpHost;
    config.server.port = smtpPort;
}


// Function to return the current time as a string
String MailService::getCurrentTime() {
    // Get the current time from the system clock
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    // Format the time into a readable string
    char buffer[30];  // Buffer size of 30 is enough for formatted time string
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Return the formatted time as a String
    return String(buffer);
}



void MailService::sendEmail(float previousDistance, float distance, float lux, bool inDeliveryWindow) {
    // Set up the email message
    String currentTime = getCurrentTime();

    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = senderEmail;
    message.subject = F("You've Got Mail!");
    // Add recipients from the list
    for (int i = 0; i < numRecipients; i++) {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

    // Create the HTML message
    String htmlMsg = "<p>Time: " + currentTime + "</p>" // Include the current time
                     "<p>Previous distance: " + String(previousDistance) + " cm</p>"
                     "<p>Current distance: " + String(distance) + " cm</p>"
                     "<p>Lux: " + String(lux) + "</p>"
                     "<p>RSSI: " + String(WiFi.RSSI()) + " dBm</p>"
                     "<p>Delivery Window Status: " + String(inDeliveryWindow ? "High" : "Low") + " Alert.</p>";
    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");
    // Connect to SMTP server
    int r = smtp.connect(&config);  // Capture the return value

    if (r == 1) {  // If successful connection (r == 1)
            Serial.println("SMTP connection successful.");
        } else {  // If connection failed
            Serial.printf("SMTP connection failed. Error code: %d\n", r);
            String errorReason = smtp.errorReason();
            if (errorReason.length() > 0) {
                Serial.printf("Connection error: %s\n", errorReason.c_str());
            }
            return;  // Exit the function if connection fails
        }

        // Send email
        if (!MailClient.sendMail(&smtp, &message)) {
            Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
        } else {
            Serial.println("Email sent successfully!");
        }
    }

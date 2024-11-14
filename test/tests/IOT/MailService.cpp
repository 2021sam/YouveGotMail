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


void MailService::sendEmail(float previousDistance, float distance, float lux, bool inDeliveryWindow) {
    // Set up the email message
    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = senderEmail;
    message.subject = F("You've Got Mail!");

    // Add recipients from the list
    for (int i = 0; i < numRecipients; i++) {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

    // Create the HTML message
    String htmlMsg = "<p>Previous distance: " + String(previousDistance) + " cm</p>"
                     "<p>Current distance: " + String(distance) + " cm</p>"
                     "<p>Lux: " + String(lux) + "</p>"
                     "<p>RSSI: " + String(WiFi.RSSI()) + " dBm</p>"
                     "<p>Delivery Window Status: " + String(inDeliveryWindow ? "High" : "Low") + " Alert.</p>";
    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");

    // Connect to SMTP server
    if (!smtp.connect(&config)) {
        Serial.printf("Connection error: %s\n", smtp.errorReason().c_str());
        return;
    }

    // Send email
    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
    } else {
        Serial.println("Email sent successfully!");
    }
}

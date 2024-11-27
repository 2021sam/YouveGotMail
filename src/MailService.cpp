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


void MailService::sendEmail(const String& htmlMsg) {
    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = senderEmail;
    message.subject = F("You've Got Mail!");
    // Add recipients from the list
    for (int i = 0; i < numRecipients; i++) {
        message.addRecipient(F("Recipient"), recipients[i]);
    }

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

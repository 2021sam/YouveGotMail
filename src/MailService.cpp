#include "MailService.h"

MailService::MailService(const char *senderEmail, const char *senderPassword, const char *smtpHost, int smtpPort,
                         const char *recipients[], int numRecipients)
{
    this->senderEmail = strdup(senderEmail);
    this->senderPassword = strdup(senderPassword);
    this->smtpHost = strdup(smtpHost);
    this->smtpPort = smtpPort;
    // this->recipients = recipients;
    this->numRecipients = numRecipients;
    Serial.println("MailService - Constructor");
    Serial.println(this->senderEmail);

    // Initialize the recipients array
    this->recipients = new const char *[numRecipients];
    for (int i = 0; i < numRecipients; i++)
    {
        if (recipients[i] != nullptr && strlen(recipients[i]) > 0)
        {
            this->recipients[i] = strdup(recipients[i]); // Duplicate strings for safety
            Serial.print("Recipient added: ");
            Serial.println(this->recipients[i]);
        }
        else
        {
            this->recipients[i] = nullptr; // Handle empty or null recipients
            Serial.print("Skipping empty recipient for index: ");
            Serial.println(i);
        }
    }

    // Initialize the session configuration
    config.login.email = senderEmail;
    config.login.password = senderPassword;
    config.server.host_name = smtpHost;
    config.server.port = smtpPort;
}


MailService::~MailService() {
    // Free dynamically allocated memory for the recipients array
    if (recipients) {
        delete[] recipients;
        recipients = nullptr; // Avoid dangling pointer
    }
    
    // Optional: Add any cleanup for SMTP session if needed
    smtp.closeSession();
    
    Serial.println("MailService destructor called. Resources freed.");
}


bool MailService::sendEmail(const String &htmlMsg)
{
    Serial.println("sendEmail");
    Serial.println(senderEmail);

    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = senderEmail;
    message.subject = F("You've Got Mail!");

    for (int i = 0; i < numRecipients; i++)
    {
        if (recipients[i] != nullptr && strlen(recipients[i]) > 0)
        { // Check if the recipient email is not blank
            Serial.print("Adding recipient: ");
            Serial.println(recipients[i]);
            message.addRecipient(F("Recipient"), recipients[i]);
        }
        else
        {
            Serial.print("Skipping empty recipient for index: ");
            Serial.println(i);
        }
    }

    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");
    // Connect to SMTP server
    int r = smtp.connect(&config); // Capture the return value

    if (r == 1)
    { // If successful connection (r == 1)
        Serial.println("SMTP connection successful.");
    }
    else
    { // If connection failed
        Serial.printf("SMTP connection failed. Error code: %d\n", r);
        String errorReason = smtp.errorReason();
        if (errorReason.length() > 0)
        {
            Serial.printf("Connection error: %s\n", errorReason.c_str());
        }
        return false; // Exit the function if connection fails
    }

    // Send email
    if (!MailClient.sendMail(&smtp, &message))
    {
        Serial.printf("Error sending email: %s\n", smtp.errorReason().c_str());
        return false;
    }
    else
    {
        Serial.println("Email sent successfully!");
        return true;
    }
}

#include "MailService.h"

MailService::MailService(const char* senderEmail, const char* senderPassword, const char* smtpHost, int smtpPort, 
                         const char* recipients[], int numRecipients) {
    this->senderEmail = senderEmail;
    this->senderPassword = senderPassword;
    this->smtpHost = smtpHost;
    this->smtpPort = smtpPort;
    // this->recipients = recipients;
    this->numRecipients = numRecipients;
    Serial.println("MailService - Constructor");


    Serial.println(this->senderEmail);


  // Initialize recipients
        for (int i = 0; i < 1; i++) {
            Serial.println(recipients[i]);
            this->recipients[i] = recipients[i];
            Serial.print("this->");
            Serial.println(this->recipients[i]);
        }




    // Initialize the session configuration
    config.login.email = senderEmail;
    config.login.password = senderPassword;
    config.server.host_name = smtpHost;
    config.server.port = smtpPort;
}


void MailService::sendEmail(const String& htmlMsg) {
    Serial.println("sendEmail");
    Serial.println(senderEmail);

    SMTP_Message message;
    message.sender.name = F("ESP Mail");
    message.sender.email = senderEmail;
    // message.sender.email = F("2020sentinel@gmail.com");


    message.subject = F("You've Got Mail!");

    // // Add recipients from the list
    // Serial.print("numRecipients: ");
    // Serial.println(numRecipients);
    // for (int i = 0; i < numRecipients; i++) {
    //     message.addRecipient(F("Recipient"), recipients[i]);
        
    // }


// Serial.print("numRecipients: ");
// Serial.println(numRecipients);

// if (recipients[0] != nullptr && strlen(recipients[0]) > 0) {
//     Serial.println(recipients[0]);
// } else {
//     Serial.println("Recipient[0] is empty or null.");
// }

//     numRecipients = 1;

// for (int i = 0; i < numRecipients; i++) {
//     if (recipients[i] != nullptr && strlen(recipients[i]) > 0) {  // Check if the recipient email is not blank
//         Serial.print("Adding recipient: ");
//         Serial.println(recipients[i]);
//         message.addRecipient(F("Recipient"), recipients[i]);
//     } else {
//         Serial.print("Skipping empty recipient for index: ");
//         Serial.println(i);
//     }
// }
//     Serial.println("CCCCCCCCCCCCCCCCCC");
   message.addRecipient(F("Recipient"),  ("2018supersam@gmail.com"));  // Hardcode recip

    message.html.content = htmlMsg;
    message.html.charSet = F("utf-8");
    // Connect to SMTP server
    int r = smtp.connect(&config);  // Capture the return value


// Before sending email, check if senderEmail is valid
Serial.print("Sender Email: ");
Serial.println(senderEmail);  // Ensure the sender email is correct

// Check SMTP settings before sending
Serial.print("SMTP Server: ");
Serial.println(smtpHost);
Serial.print("SMTP Port: ");
Serial.println(smtpPort);




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

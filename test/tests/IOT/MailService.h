#ifndef MAILSERVICE_H
#define MAILSERVICE_H

#include <ESP_Mail_Client.h>

class MailService {
public:
    MailService(const char* senderEmail, const char* senderPassword, const char* smtpHost, int smtpPort, 
                const char* recipients[], int numRecipients);
    void sendEmail(float previousDistance, float distance, float lux, bool inDeliveryWindow);

private:
    const char* senderEmail;
    const char* senderPassword;
    const char* smtpHost;
    int smtpPort;
    const char** recipients;
    int numRecipients;
    
    // These are your class members that hold the SMTP session and configuration
    SMTPSession smtp;    // Declared as a class member (not local in the function)
    // SMTP_Config config;  // Declared as a class member (not local in the function)
    Session_Config config;
};

#endif

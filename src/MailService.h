#ifndef MAILSERVICE_H
#define MAILSERVICE_H

#include <ESP_Mail_Client.h>

class MailService {
public:
    MailService(const char* senderEmail, const char* senderPassword, const char* smtpHost, int smtpPort, 
                const char* recipients[], int numRecipients);

    // Destructor
    ~MailService();

    bool sendEmail(const String& htmlMsg);


// private:

    const char* senderEmail;
    const char* senderPassword;
    const char* smtpHost;
    int smtpPort;
    const char** recipients;
    // const char* recipients[3];
    int numRecipients;
    
    // These are your class members that hold the SMTP session and configuration
    SMTPSession smtp;    // Declared as a class member (not local in the function)
    Session_Config config;
};

#endif

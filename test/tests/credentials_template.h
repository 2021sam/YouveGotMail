#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "Your_SSID"
#define WIFI_PASSWORD "Your_Password"
#define AUTHOR_EMAIL "your_email@example.com"
#define AUTHOR_PASSWORD "your_app_password"

// SMTP constants
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587

// Recipients list
#define NUM_RECIPIENTS 3 // Adjust the number based on your needs

const char* recipients[NUM_RECIPIENTS] = {
    "recipient1@example.com",
    "recipient2@example.com",
    "recipient3@example.com"
};


#endif // CREDENTIALS_H

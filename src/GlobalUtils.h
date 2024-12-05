#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

#include "MailService.h"
#include <Arduino.h>
#include <ctime>

// Function prototype for getCurrentTime
String getCurrentTime();

// Function prototype for addToLog
void addToLog(String message);

// Declare global variables using extern to avoid multiple definitions
extern bool isMailboxOpen;
extern int openThreshold;
extern bool mail_alert;
extern float currentLux;
extern float currentTof;
extern int currentRSSI;
extern float previousTof;
extern float minLux, maxLux;
extern float minTof, maxTof, maxTof_inClosedState;
extern int minRSSI, maxRSSI;
// Global constant for time formatting buffer size
const int TIME_BUFFER_SIZE = 30;
extern String systemLog[20];  // Array to hold the log entries
extern int logIndex;  // Index to track where to add the new log entry

// Struct to hold configuration settings
struct ConfigSettings {
    String wifiHostname;       // Added field for Wi-Fi hostname
    String recipientEmail1;
    String recipientEmail2;
    String recipientEmail3;
    String smtpHost;
    int smtpPort;
    String authorEmail;
    String authorPassword;
};

// Function prototypes for Wi-Fi and configuration management
void deleteWiFiCredentials();
void saveWiFiCredentials(const String& ssid, const String& password);
void loadWiFiCredentials(String& ssid, String& password);
ConfigSettings loadConfigSettings();  // Retrieve configuration settings

// Declare functions to manage Wi-Fi and config settings
void saveWiFiCredentials(const String &ssid, const String &password);
void loadWiFiCredentials(String &ssid, String &password);

void saveConfigSettings(const String &email1, const String &email2, const String &email3,
                        const String &smtpHost, int smtpPort,
                        const String &authorEmail, const String &authorPassword,
                        const String &wifiHostname);

void loadConfigSettings(String &email1, String &email2, String &email3,
                        String &smtpHost, int &smtpPort,
                        String &authorEmail, String &authorPassword,
                        String &wifiHostname);

void updateSensorValues(int rssi, float lux, float tof);

MailService* getMailService();
// Declare a global pointer to MailService
extern MailService* globalMailService;  

// Function to initialize the global MailService
void initializeMailService(
    const char* senderEmail,
    const char* senderPassword,
    const char* smtpHost,
    int smtpPort,
    const char* recipients[],
    int numRecipients
);

// GlobalUtils.h
void initializeGlobalMailService();


// Function to send an email using the global MailService
void sendGlobalEmail(const String& htmlMsg);

#endif // GLOBAL_UTILS_H

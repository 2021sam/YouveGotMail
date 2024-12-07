#include "GlobalUtils.h"  // Include the header to match the declarations
#include <Preferences.h>

// Define the global variables (memory allocation)
bool isMailboxOpen = false;
int openThreshold = 5;
bool mail_alert = false;
float currentLux = 0.0;
float currentTof = 0.0;
int currentRSSI = 0;
float previousTof = 0.0;
float minLux = 1e6, maxLux = -1e6;
float minTof = 1e6, maxTof = -1e6, maxTof_inClosedState = -1e6;
int minRSSI = 32767, maxRSSI = -32768;
// Declare and initialize the global variable
MailService* globalMailService = nullptr;

String systemLog[20];  // Array to hold the log entries
int logIndex = 0;      // Index to track where to add the new log entry

Preferences preferences;  // Preferences instance for configuration storage

// Define the getCurrentTime function (only defined here in GlobalUtils.cpp)
String getCurrentTime() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    const int TIME_BUFFER_SIZE = 30;
    char buffer[TIME_BUFFER_SIZE];  // Buffer to hold the formatted time
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}

// Define the addToLog function (only defined here in GlobalUtils.cpp)
void addToLog(String message) {
    String timestamp = getCurrentTime();  // Get current time via the Alert system
    String logMessage = "[" + timestamp + "] " + message + "\n";  // Add a timestamp and message to the log
    Serial.println(logMessage);  // Print message to serial monitor for debugging
    systemLog[logIndex] = logMessage;
    logIndex = (logIndex + 1) % 10;  // Wrap around after 10 entries
}

// Function to update current sensor values and min/max values
void updateSensorValues(int rssi, float lux, float tof) {
    // Update current sensor values
    currentLux = lux;
    currentTof = tof;
    currentRSSI = rssi;

    if (isMailboxOpen){
        if (currentTof < maxTof_inClosedState){
            isMailboxOpen = false;
            addToLog("Mailbox Closed");
        }
    }
    else
    {
        if (currentTof < previousTof + 1)
        {
            if (tof > maxTof_inClosedState)
                maxTof_inClosedState = tof;
        }

        if (currentTof > previousTof + openThreshold)
        {
            isMailboxOpen = true;
            mail_alert = true;
            addToLog("Mailbox Open");
            // addToLog("Max ToF in Closed State: " + String(maxTof_inClosedState) + " cm");
        }

    }

    // Update previous ToF value
    previousTof = currentTof;

    // Update min/max values
    if (lux < minLux) minLux = lux;
    if (lux > maxLux) maxLux = lux;

    if (tof < minTof) minTof = tof;
    if (tof > maxTof) maxTof = tof;

    if (rssi < minRSSI) minRSSI = rssi;
    if (rssi > maxRSSI) maxRSSI = rssi;
}


// Function to delete Wi-Fi credentials
void deleteWiFiCredentials() {
    preferences.begin("wifi", false);
    preferences.remove("wifi_ssid");
    preferences.remove("wifi_password");
    preferences.end();
    Serial.println("Wi-Fi credentials erased.");
}

// Save Wi-Fi credentials
void saveWiFiCredentials(const String& ssid, const String& password) {
    preferences.begin("wifi", false);
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_password", password);
    preferences.end();
    Serial.println("Wi-Fi credentials saved.");
}

// Load Wi-Fi credentials
void loadWiFiCredentials(String& ssid, String& password) {
    preferences.begin("wifi", true);
    ssid = preferences.getString("wifi_ssid", "");
    password = preferences.getString("wifi_password", "");
    preferences.end();
}

// Load full configuration settings
ConfigSettings loadConfigSettings() {
    ConfigSettings config;
    preferences.begin("config", true);
    config.recipientEmail1 = preferences.getString("recip_email_1", "");
    config.recipientEmail2 = preferences.getString("recip_email_2", "");
    config.recipientEmail3 = preferences.getString("recip_email_3", "");
    config.smtpHost = preferences.getString("smtp_host", "smtp.gmail.com");
    config.smtpPort = preferences.getInt("smtp_port", 587);
    config.wifiHostname = preferences.getString("wifi_hostname", "Got_Mail");
    config.authorEmail = preferences.getString("author_email", "");
    config.authorPassword = preferences.getString("author_password", "");
    preferences.end();

    Serial.println("Loaded Config Settings:");
    Serial.println("Author Email: " + config.authorEmail);
    Serial.println("SMTP Host: " + config.smtpHost);
    Serial.println("SMTP Port: " + String(config.smtpPort));
    Serial.println("Recipient 1: " + config.recipientEmail1);
    Serial.println("Recipient 2: " + config.recipientEmail2);
    Serial.println("Recipient 3: " + config.recipientEmail3);
    return config;
}


void saveConfigSettings(const String& email1, const String& email2, const String& email3,
                        const String& smtpHost, int smtpPort, 
                        const String& authorEmail, const String& authorPassword, 
                        const String& wifiHostname) {
    // Save settings logic
    Serial.println("Saving configuration settings...");
    preferences.begin("config", false);  // Open preferences in write mode

    preferences.putString("recip_email_1", email1);
    preferences.putString("recip_email_2", email2);
    preferences.putString("recip_email_3", email3);
    preferences.putString("smtp_host", smtpHost);
    preferences.putInt("smtp_port", smtpPort);
    preferences.putString("author_email", authorEmail);
    preferences.putString("author_password", authorPassword);
    preferences.putString("wifi_hostname", wifiHostname);

    preferences.end();  // Close preferences
    Serial.println("Configuration saved successfully.");
}


void sendGlobalEmail(const String& htmlMsg) {
    if (globalMailService) {
        globalMailService->sendEmail(htmlMsg);
    } else {
        Serial.println("Error: MailService is not initialized!");
    }
}
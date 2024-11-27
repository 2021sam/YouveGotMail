#include "GlobalUtils.h"  // Include the header to match the declarations

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

String systemLog[10];  // Array to hold the log entries
int logIndex = 0;      // Index to track where to add the new log entry


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
            addToLog("Mailbox state changed to: Closed");
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
            addToLog("Mailbox state changed to: Open");
            addToLog("Max ToF in Closed State: " + String(maxTof_inClosedState) + " cm");
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

#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

#include <Arduino.h>
#include <ctime>

// Global constant for time formatting buffer size
const int TIME_BUFFER_SIZE = 30;
// Log storage (simple circular buffer or array)
String systemLog[10];  // Array to hold the log entries
int logIndex = 0;  // Index to track where to add the new log entry

// Global utility function to get the current time as a formatted string
String getCurrentTime() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    char buffer[TIME_BUFFER_SIZE];  // Buffer to hold the formatted time
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return String(buffer);
}


void addToLog(String message) {
    // Add a log entry and maintain a simple circular buffer
    String timestamp = getCurrentTime();  // Get current time via the Alert system
    String logMessage = "[" + timestamp + "] " + message + "\n";  // Add a timestamp and message to the log
    Serial.println(logMessage);  // Print message to serial monitor for debugging

    systemLog[logIndex] = logMessage;
    logIndex = (logIndex + 1) % 10;  // Wrap around after 10 entries
}


// // Global utility function to calculate the absolute difference between two floats
// float calculateAbsoluteDifference(float value1, float value2) {
//     return abs(value1 - value2);
// }

// // Global utility function to check if the current hour is within a given range
// bool isCurrentHourWithinRange(int startHour, int endHour) {
//     time_t now = time(nullptr);
//     struct tm* timeinfo = localtime(&now);
//     int currentHour = timeinfo->tm_hour;
//     return (currentHour >= startHour && currentHour < endHour);
// }

#endif // GLOBAL_UTILS_H

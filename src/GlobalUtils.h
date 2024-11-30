#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

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

// Function prototypes
void updateSensorValues(int rssi, float lux, float tof);

#endif // GLOBAL_UTILS_H

#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H

#include "TFT_eSPI.h"  // Use the correct TFT library for your display

class TFTDisplay {
public:
    TFTDisplay();  // Constructor

    // Initialize the display
    void begin();

    // Show all data (distance, lux, RSSI, time)
    void showAllData(float distance, float lux, int rssi, const String& currentTime);
    void showStatusMessage(String statusMessage);  // Display the status message

private:
    TFT_eSPI tft;  // TFT instance

    // Utility functions
    void clearScreen();
    void drawText(const String& text, int x, int y, int textSize = 2);
};

#endif

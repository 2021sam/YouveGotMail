#include "TFTDisplay.h"

// Constructor
TFTDisplay::TFTDisplay() : tft() {}

// Initialize the TFT display
void TFTDisplay::begin() {
    tft.init();
    tft.setRotation(3);  // Adjust based on your orientation preference
    tft.fillScreen(TFT_BLACK);  // Start with a black screen
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
}

// Show all data (distance, lux, RSSI, current time) on one screen
void TFTDisplay::showAllData(float distance, float lux, int rssi, const String& currentTime) {
    clearScreen();  // Clear the screen before displaying new data

    // Display all data on the screen
    drawText("Distance: " + String(distance) + " cm", 10, 10);
    drawText("Light Level: " + String(lux) + " lux", 10, 40);
    drawText("Wi-Fi RSSI: " + String(rssi) + " dBm", 10, 70);
    drawText("Time: " + currentTime, 10, 100);
}

// Utility functions

void TFTDisplay::clearScreen() {
    tft.fillScreen(TFT_BLACK);  // Clear the screen with black
}

void TFTDisplay::drawText(const String& text, int x, int y, int textSize) {
    tft.setTextSize(textSize);
    tft.setCursor(x, y);
    tft.println(text);
}


// Display the status message from Alert on the screen
void TFTDisplay::showStatusMessage(String statusMessage) {
    clearScreen();  // Clear the screen before showing the message

    // tft.setTextSize(3);  // Set text size to 3 (or any other size)
    int lineHeight = 30;  // Adjust line height based on text size (size 3 will need more space)
    int maxWidth = tft.width() - 20;  // Max width of the text (with some padding)
    int startY = 10;  // Starting Y position

    String currentLine = "";  // Store current line of text
    String word = "";  // Store each word temporarily
    int lineCount = 0;  // To keep track of lines

    // Loop through each character in the statusMessage
    for (int i = 0; i < statusMessage.length(); i++) {
        char c = statusMessage[i];

        // If we encounter a space, it means we have a word ready to print
        if (c == ' ') {
            // Check if adding the word exceeds the max width
            if (tft.textWidth(currentLine + word) > maxWidth) {
                // If it exceeds, print the current line
                tft.setCursor(10, startY + (lineCount * lineHeight));
                tft.print(currentLine);
                lineCount++;  // Move to the next line

                // If we've reached the maximum number of lines, stop
                if (lineCount >= 5) {
                    break;
                }

                // Reset the current line to the word that couldn't fit
                currentLine = word;
            } else {
                // If it fits, add the word to the current line
                currentLine += word + " ";
            }
            word = "";  // Clear the word after adding it to the current line
        } else {
            word += c;  // Build up the word character by character
        }
    }

    // After the loop, there may be one final word left
    if (word.length() > 0) {
        if (tft.textWidth(currentLine + word) > maxWidth) {
            // If the last word doesn't fit, print the current line and start a new one
            tft.setCursor(10, startY + (lineCount * lineHeight));
            tft.print(currentLine);
            lineCount++;  // Move to the next line
            currentLine = word;
        } else {
            // If the last word fits, add it to the current line
            currentLine += word;
        }
    }

    // Print the final line
    if (currentLine.length() > 0 && lineCount < 5) {
        tft.setCursor(10, startY + (lineCount * lineHeight));
        tft.print(currentLine);
    }
}

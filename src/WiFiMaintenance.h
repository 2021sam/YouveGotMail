#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "TFTDisplay.h"  // Include the TFTDisplay header to use it


// Function to set up Wi-Fi connection
String setup_WiFi(TFTDisplay& display) {
    Serial.println();
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);

    int blinkDelay = 500;  // Delay in milliseconds for blinking effect (flashing text)
    int lastBlinkTime = 0;  // Track time for blinking
    bool showText = true;  // Toggle to switch between showing and hiding the text

    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(5000);  // Wait for 5 seconds before retrying
        Serial.print(".");

		if (WiFi.status() != WL_CONNECTED)
		{
            // Flash the "Connecting to Wi-Fi..." text every 500ms
            unsigned long currentMillis = millis();
            if (currentMillis - lastBlinkTime >= blinkDelay) {
                lastBlinkTime = currentMillis;
                showText = !showText;  // Toggle visibility of the text

                if (showText) {
                    // Show "Connecting to Wi-Fi" message with RSSI value
                    int rssi = WiFi.RSSI();  // Get the current RSSI value
                    display.showStatusMessage("Connecting to Wi-Fi: " + String(WIFI_SSID) + " RSSI: " + String(rssi));
                } else {
                    display.showStatusMessage("");  // Clear text on screen
                }
            }


			WiFi.disconnect();  // Disconnect from any existing Wi-Fi connections
			delay(1000);  // Small delay to ensure Wi-Fi is disconnected before starting new connection
        }
        delay(1000);  // Small delay to avoid blocking completely
    }
    String ipAddress = WiFi.localIP().toString();  // Get the IP address
    Serial.println("\nWi-Fi connected");
    Serial.print("IP address: ");
    Serial.println(ipAddress);

    delay(1000);
    // Set the device's hostname after successful Wi-Fi connection
    WiFi.setHostname(WIFI_HOSTNAME);  // Set the hostname for the device
    Serial.print("Wi-Fi Hostname: ");
    Serial.println(WIFI_HOSTNAME);
    display.showStatusMessage("Wi-Fi Hostname: " + String(WIFI_HOSTNAME) );

    return ipAddress;  // Return the IP address
}

// Function to check if Wi-Fi is still connected and reconnect if needed
void checkWiFiConnection(TFTDisplay& display) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi connection lost. Reconnecting...");
        display.showStatusMessage("Wi-Fi connection lost.  Reconnecting...");
        // setup_WiFi();
        delay(1000);
        String ipAddress = setup_WiFi(display);
        display.showStatusMessage(ipAddress);
        delay(1000);
    }
}

#endif // WIFI_MANAGER_H

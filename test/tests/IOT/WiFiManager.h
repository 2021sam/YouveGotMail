#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "credentials.h"  // Ensure this includes your Wi-Fi credentials

// Function to set up Wi-Fi connection
void setup_WiFi() {
    Serial.println();
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);  // Wait for 5 seconds before retrying
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

// Function to check if Wi-Fi is still connected and reconnect if needed
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi connection lost. Reconnecting...");
        setup_WiFi();
    }
}

#endif // WIFI_MANAGER_H

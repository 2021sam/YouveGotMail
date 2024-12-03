#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>  // Include Preferences to store Wi-Fi credentials
#include <WiFiManager.h>  // Include WiFiManager to handle AP mode
#include "TFTDisplay.h"  // Include the TFTDisplay header to use it

Preferences preferences;  // Create an instance of the Preferences class



void deleteWiFiCredentials() {
    preferences.begin("wifi", false);
    preferences.remove("ssid");
    preferences.remove("password");
    preferences.end();
    Serial.println("Wi-Fi credentials erased.");
}


// Function to load Wi-Fi credentials from Preferences
void loadWiFiCredentials(String &ssid, String &password) {
    preferences.begin("wifi", false);  // Open "wifi" namespace for reading credentials
    ssid = preferences.getString("ssid", "");  // Read SSID, or use default if not set
    password = preferences.getString("password", "");  // Read password, or use default
    preferences.end();  // Close Preferences
}

// Function to save Wi-Fi credentials to Preferences
void saveWiFiCredentials(const String &ssid, const String &password) {
    preferences.begin("wifi", false);  // Open "wifi" namespace for writing credentials
    preferences.putString("ssid", ssid);  // Save SSID
    preferences.putString("password", password);  // Save password
    preferences.end();  // Close Preferences
}


// Function to start AP mode and let the user configure Wi-Fi credentials
void startAPMode() {
    Serial.println("Entering AP mode...");

    WiFiManager wifiManager;

    // Set the callback to save credentials once they are entered
    wifiManager.setSaveConfigCallback([]() {
        String savedSSID = WiFi.SSID();
        String savedPassword = WiFi.psk();
        saveWiFiCredentials(savedSSID, savedPassword);
        Serial.println("Updated new WiFi Credentials.");
    });

    // Start the configuration portal
    if (!wifiManager.startConfigPortal("ESP_AP_Config")) {
        Serial.println("Failed to connect or configure Wi-Fi.");
        delay(3000);
        ESP.restart(); // Restart if configuration fails
    }

    // After the user configures the Wi-Fi, you can proceed with your normal setup
    Serial.println("WiFi configured successfully.");
}


// Function to set up Wi-Fi connection
String setup_WiFi(TFTDisplay& display)
{
    Serial.println("Load Credentials from EEPROM Preferences");
    String WIFI_SSID, WIFI_PASSWORD;
    loadWiFiCredentials(WIFI_SSID, WIFI_PASSWORD);  // Load saved Wi-Fi credentials
    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASSWORD);

    if (WIFI_SSID.isEmpty()) {
        display.showStatusMessage("Web Portal AP started: ESP_AP_Config, AP IP address: 192.168.4.1");
        delay(2000);
        startAPMode();
        loadWiFiCredentials(WIFI_SSID, WIFI_PASSWORD);  // Load saved Wi-Fi credentials
        // display.showStatusMessage("Connecting to Wi-Fi: " + String(WIFI_SSID) + ", PASSWORD: " + String(WIFI_PASSWORD));
    }
 display.showStatusMessage("Connecting to Wi-Fi: " + String(WIFI_SSID) + ", PASSWORD: " + String(WIFI_PASSWORD));
    // Serial.println();
    // Serial.println("Connecting to Wi-Fi: ");
    // Serial.println(WIFI_SSID);
    // Serial.println(WIFI_PASSWORD);
    // display.showStatusMessage("Connecting to Wi-Fi: " + String(WIFI_SSID) + ", PASSWORD: " + String(WIFI_PASSWORD));
    delay(3000);
    int blinkDelay = 500;  // Delay in milliseconds for blinking effect (flashing text)
    int lastBlinkTime = 0;  // Track time for blinking
    bool showText = true;  // Toggle to switch between showing and hiding the text

    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(500);  // Wait for 5 seconds before retrying
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
                    display.showStatusMessage("Wi-Fi: " + String(WIFI_SSID) + " RSSI: " + String(rssi));
                     delay(3000);
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
    // WiFi.setHostname(WIFI_HOSTNAME);  // Set the hostname for the device
    // Serial.print("Wi-Fi Hostname: ");
    // Serial.println(WIFI_HOSTNAME);
    // display.showStatusMessage("Wi-Fi Hostname: " + String(WIFI_HOSTNAME) );

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

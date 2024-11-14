#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <LittleFS.h>   // Use LittleFS for file storage
#include <WebServer.h>  // Web Server library for serving HTTP requests
#include "credentials.h" // Include your credentials for Wi-Fi setup

#define LED_PIN 21

WebServer server(80);  // Create a WebServer object that listens on port 80

// File system in use (LittleFS)
fs::FS *fsys = &LittleFS;

// Static content (HTML) to display device data
const char pageContent[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Device Information</title>
  <style>
    body { font-family: Arial, sans-serif; }
    h1 { color: #2E8B57; }
    table { width: 50%; margin: 25px 0; border-collapse: collapse; }
    th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }
  </style>
</head>
<body>
  <h1>Device Information</h1>
  <table>
    <tr><th>Chip Model</th><td>{{chip_model}}</td></tr>
    <tr><th>Chip Revision</th><td>{{chip_revision}}</td></tr>
    <tr><th>CPU Cores</th><td>{{cpu_cores}}</td></tr>
    <tr><th>Free Heap</th><td>{{free_heap}}</td></tr>
    <tr><th>Flash Size</th><td>{{flash_size}}</td></tr>
    <tr><th>Free File System Space</th><td>{{fs_free_space}} bytes</td></tr>
  </table>
</body>
</html>
)";

// Wi-Fi setup function using credentials from credentials.h
void setup_WiFi()
{
    Serial.println();
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID); // From credentials.h

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Use Wi-Fi credentials from credentials.h

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(5000);  // Wait for 5 seconds before trying again
        Serial.print("...");
    }
    Serial.println("\nWi-Fi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// This function is called when the WebServer is requested without giving a filename
// This will show the device data page
void handleRoot() {
  // Collect device information
  String page = pageContent;
  page.replace("{{chip_model}}", String(ESP.getChipModel()));
  page.replace("{{chip_revision}}", String(ESP.getChipRevision()));
  page.replace("{{cpu_cores}}", String(ESP.getChipCores()));
  page.replace("{{free_heap}}", String(ESP.getFreeHeap()));
  page.replace("{{flash_size}}", String(ESP.getFlashChipSize()));
  page.replace("{{fs_free_space}}", String(LittleFS.totalBytes() - LittleFS.usedBytes()));

  // Send the response to the client (browser)
  server.send(200, "text/html", page);
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // Setup Wi-Fi connection
    setup_WiFi();

    // Mount the LittleFS filesystem
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed.");
        return;
    }

    Serial.println("LittleFS mounted successfully!");

    // Setup web server
    server.on("/", HTTP_GET, handleRoot);  // Root endpoint for device info page

    // Start the server
    server.begin();
    Serial.println("WebServer started");
}

void loop()
{
    // Handle incoming HTTP requests
    server.handleClient();
}

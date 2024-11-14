// #include <Arduino.h>
// #include <WebServer.h>
// #include <WiFi.h>
// #include "credentials.h"  // Include credentials.h for Wi-Fi credentials
// #include <FS.h>        // File System for Web Server Files
// #include <LittleFS.h>  // Use LittleFS file system

// // Trace output for debugging
// #define TRACE(...) Serial.printf(__VA_ARGS__)

// // Web Server on port 80
// WebServer server(80);

// // File system in use (LittleFS or FAT)
// fs::FS *fsys = nullptr;

// // Static content (HTML) to display device data
// const char pageContent[] PROGMEM = R"(
// <!DOCTYPE html>
// <html lang="en">
// <head>
//   <meta charset="UTF-8">
//   <meta name="viewport" content="width=device-width, initial-scale=1.0">
//   <title>Device Information</title>
//   <style>
//     body { font-family: Arial, sans-serif; }
//     h1 { color: #2E8B57; }
//     table { width: 50%; margin: 25px 0; border-collapse: collapse; }
//     th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }
//   </style>
// </head>
// <body>
//   <h1>Device Information</h1>
//   <table>
//     <tr><th>Chip Model</th><td>{{chip_model}}</td></tr>
//     <tr><th>Chip Revision</th><td>{{chip_revision}}</td></tr>
//     <tr><th>CPU Cores</th><td>{{cpu_cores}}</td></tr>
//     <tr><th>Free Heap</th><td>{{free_heap}}</td></tr>
//     <tr><th>Flash Size</th><td>{{flash_size}}</td></tr>
//     <tr><th>Free File System Space</th><td>{{fs_free_space}} bytes</td></tr>
//   </table>
// </body>
// </html>
// )";

// // This function is called when the WebServer is requested without giving a filename
// // This will redirect to the file index.htm if it exists, or show the device data page
// void handleRoot() {
//   TRACE("Serving device data...\n");

//   // Collect device information
//   String page = pageContent;
//   page.replace("{{chip_model}}", String(ESP.getChipModel()));
//   page.replace("{{chip_revision}}", String(ESP.getChipRevision()));
//   page.replace("{{cpu_cores}}", String(ESP.getChipCores()));
//   page.replace("{{free_heap}}", String(ESP.getFreeHeap()));
//   page.replace("{{flash_size}}", String(ESP.getFlashChipSize()));
//   page.replace("{{fs_free_space}}", String(LittleFS.totalBytes() - LittleFS.usedBytes()));

//   // Send the response to the client (browser)
//   server.send(200, "text/html", page);
// }

// // This function returns the Wi-Fi RSSI in JSON format
// void handleRSSI() {
//   int32_t rssi = WiFi.RSSI();  // Get RSSI of the current Wi-Fi connection

//   // Prepare the JSON response
//   String jsonResponse = "{\"rssi\": " + String(rssi) + "}";

//   // Send JSON response
//   server.send(200, "application/json", jsonResponse);
// }

// // Setup the web server and WiFi
// void setup(void) {
//   delay(3000);  // Wait for serial monitor to start

//   // Start Serial
//   Serial.begin(115200);
//   Serial.setDebugOutput(false);

//   TRACE("Starting WebServer...\n");

//   // Mount file system
//   if (!LittleFS.begin()) {
//     TRACE("Failed to mount LittleFS\n");
//     return;
//   }

//   fsys = &LittleFS;

//   // Connect to WiFi using credentials from credentials.h
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   TRACE("Connecting to WiFi...\n");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     TRACE(".");
//   }
//   TRACE("Connected to WiFi\n");

//   // Register service for the root path ('/')
//   server.on("/", HTTP_GET, handleRoot);

//   // Register service for the RSSI endpoint
//   server.on("/rssi", HTTP_GET, handleRSSI);

//   // Start the server
//   server.begin();
//   TRACE("Server started. Visit http://%s\n", WiFi.localIP().toString().c_str());
// }

// // Main loop
// void loop(void) {
//   server.handleClient();
// }

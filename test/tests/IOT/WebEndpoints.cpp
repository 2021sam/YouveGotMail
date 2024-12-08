// IOT\WebEndpoints.cpp

// #include <LittleFS.h>  // Or any file system you're using
#include "WebEndpoints.h"  // Include the header file for WebEndpoints
#include "index_html.h" // Include the HTML content

// Constructor
WebEndpoints::WebEndpoints(WebServer& server, MailService* mailService)
    : server(server), mailService(mailService) {
    // Define the routes and associate them with the corresponding handler functions
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/device", HTTP_GET, [this](){ handleDeviceInfo(); }); // Use the new device info endpoint
    server.on("/distance", HTTP_GET, [this]() { handleDistance(); });
    server.on("/lux", HTTP_GET, [this]() { handleLux(); });
    server.on("/rssi", HTTP_GET, [this]() { handleRSSI(); });
    server.on("/alert", HTTP_GET, [this]() { handleEmailAlert(); });
    }



    // Handle the root page
    void WebEndpoints::handleRoot() {
        server.send(200, "text/html", pageContent);
    }


void WebEndpoints::handleDeviceInfo() {
// File system (LittleFS or FAT)
    // if (!LittleFS.begin()) {
    //     server.send(500, "text/html", "Error accessing file system.");
    //     return;
    // }

    // Calculate free space
    // size_t freeSpace = LittleFS.totalBytes() - LittleFS.usedBytes();

    // fs::FS *fsys = nullptr;

    // Create the response string with HTML content and styles
    String response = "<html><head><style>"
                      "body { font-family: Arial, sans-serif; }"
                      "h1 { color: #2E8B57; }"
                      "table { width: 50%; margin: 25px 0; border-collapse: collapse; }"
                      "th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }"
                      "</style></head><body>";

    response += "<h1>Device Information</h1>";
    response += "<table>";
    response += "<tr><th>Chip Model</th><td>" + String(ESP.getChipModel()) + "</td></tr>";
    response += "<tr><th>Chip Revision</th><td>" + String(ESP.getChipRevision()) + "</td></tr>";
    response += "<tr><th>CPU Cores</th><td>" + String(ESP.getChipCores()) + "</td></tr>";
    response += "<tr><th>Free Heap</th><td>" + String(ESP.getFreeHeap()) + " bytes</td></tr>";
    response += "<tr><th>Flash Size</th><td>" + String(ESP.getFlashChipSize()) + " bytes</td></tr>";
    // response += "<tr><th>Free File System Space</th><td>" + String(LittleFS.totalBytes() - LittleFS.usedBytes()) + " bytes</td></tr>";
    response += "</table>";
    response += "</body></html>";

    // Send the response to the client (browser) as HTML
    server.send(200, "text/html", response);
}



// Handle the distance endpoint
void WebEndpoints::handleDistance() {
    float distance = distanceSensor.getDistance();
    String response = "Distance: " + String(distance) + " cm";
    server.send(200, "text/plain", response);
}

// Handle the light level endpoint
void WebEndpoints::handleLux() {
    float lux = lightSensor.getLightLevel();
    String response = "Light Level: " + String(lux) + " lux";
    server.send(200, "text/plain", response);
}

// Handle the RSSI endpoint
void WebEndpoints::handleRSSI() {
    int rssi = WiFi.RSSI();
    String response = "Wi-Fi RSSI: " + String(rssi) + " dBm";
    server.send(200, "text/plain", response);
}

// Handle the email alert endpoint
void WebEndpoints::handleEmailAlert() {

    float currentDistance = distanceSensor.getDistance();
    float lightLevel = lightSensor.getLightLevel();
    previousDistance = currentDistance;

    mailService->sendEmail(previousDistance, currentDistance, lightSensor.getLightLevel(), isWithinDeliveryWindow());
    server.send(200, "text/html", "<h1>Test Email Sent Successfully!</h1><p><a href='/'>Go back</a></p>");
}

// Check if the current time is within the delivery window
bool WebEndpoints::isWithinDeliveryWindow() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    return (currentHour >= 8 && currentHour < 17);  // Delivery window is between 8 AM and 5 PM
}

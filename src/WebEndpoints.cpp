// IOT\WebEndpoints.cpp

#include "WebEndpoints.h"  // Include the header file for WebEndpoints
#include "index_html.h" // Include the HTML content

WebEndpoints::WebEndpoints(WebServer& server, MailService* mailService, 
                           String* systemLog, int& logIndex) : server(server), mailService(mailService),
      systemLog(systemLog), logIndex(logIndex) {

    // Define the routes and associate them with the corresponding handler functions
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/device", HTTP_GET, [this](){ handleDeviceInfo(); }); // Use the new device info endpoint
    server.on("/distance", HTTP_GET, [this]() { handleDistance(); });
    server.on("/lux", HTTP_GET, [this]() { handleLux(); });
    server.on("/rssi", HTTP_GET, [this]() { handleRSSI(); });
    server.on("/alert", HTTP_GET, [this]() { handleEmailAlert(); });
    server.on("/all", HTTP_GET, [this]() { handleSensorData(); });
    server.on("/log", HTTP_GET, [this]() { handleLog(); });
    }


    // Handle the root page
    void WebEndpoints::handleRoot() {
        server.send(200, "text/html", pageContent);
    }


void WebEndpoints::handleDeviceInfo() {
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
    response += "</table>";
    response += "</body></html>";

    // Send the response to the client (browser) as HTML
    server.send(200, "text/html", response);
}


// Handle the distance endpoint
void WebEndpoints::handleDistance() {
    String response = "Distance: " + String(currentTof) + " cm";
    server.send(200, "text/plain", response);
}


void WebEndpoints::handleSensorData() {
    // Create an HTML response with auto-refresh
    String response = "<html><head><style>"
                      "body { font-family: Arial, sans-serif; }"
                      "h1 { color: #2E8B57; }"
                      "table { width: 50%; margin: 25px 0; border-collapse: collapse; }"
                      "th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }"
                      "</style></head><body>";
    
    response += "<h1>Sensor Data</h1>";
    response += "<table>";
    response += "<tr><th>Parameter</th><th>Current</th><th>Min</th><th>Max</th></tr>";
    response += "<tr><td>RSSI (dBm)</td><td>" + String(currentRSSI) + "</td><td>" + String(minRSSI) + "</td><td>" + String(maxRSSI) + "</td></tr>";
    response += "<tr><td>Light Level (Lux)</td><td>" + String(currentLux) + "</td><td>" + String(minLux) + "</td><td>" + String(maxLux) + "</td></tr>";
    response += "<tr><td>Distance (ToF cm)</td><td>" + String(currentTof) + "</td><td>" + String(minTof) + "</td><td>" + String(maxTof) + "</td></tr>";
    response += "<tr><td>Max ToF in Closed State (cm)</td><td>" + String(maxTof_inClosedState) + "</td><td>N/A</td><td>N/A</td></tr>";
    response += "</table>";
    
    // JavaScript to auto-refresh the page every second (1000 milliseconds)
    response += "<script>setTimeout(function(){ location.reload(); }, 1000);</script>";  

    response += "</body></html>";

    // Send the response as HTML
    server.send(200, "text/html", response);
}


// Handle the light level endpoint
void WebEndpoints::handleLux() {
    String response = "Light Level: " + String(currentLux) + " lux";
    server.send(200, "text/plain", response);
}

// Handle the RSSI endpoint
void WebEndpoints::handleRSSI() {
    String response = "Wi-Fi RSSI: " + String(currentRSSI) + " dBm";
    server.send(200, "text/plain", response);
}

// Handle the email alert endpoint
void WebEndpoints::handleEmailAlert() {

        // Create the HTML message
        String htmlMsg = "<p>Time: " + getCurrentTime() + "</p>" // Include the current time
                        "<p>Previous distance: " + String(previousTof) + " cm</p>"
                        "<p>Current distance: " + String(currentTof) + " cm</p>"
                        "<p>Lux: " + String(currentLux) + "</p>"
                        "<p>RSSI: " + String(currentRSSI) + " dBm</p>"
                        "<p>Delivery Window Status: " + String(isWithinDeliveryWindow() ? "High" : "Low") + " Alert.</p>";

    mailService->sendEmail(htmlMsg);
    server.send(200, "text/html", "<h1>Test Email Sent Successfully!</h1><p><a href='/'>Go back</a></p>");
}

// Check if the current time is within the delivery window
bool WebEndpoints::isWithinDeliveryWindow() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentHour = timeinfo->tm_hour;
    return (currentHour >= 8 && currentHour < 17);  // Delivery window is between 8 AM and 5 PM
}


void WebEndpoints::handleLog() {
    String response = "<html><head><style>"
                      "body { font-family: Arial, sans-serif; }"
                      "h1 { color: #2E8B57; }"
                      "table { width: 50%; margin: 25px 0; border-collapse: collapse; }"
                      "th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }"
                      "</style></head><body>";

    response += "<h1>System Log</h1>";
    response += "<table>";
    
    // Display log entries
    for (int i = 0; i < 10; i++) {
        if (systemLog[i] != "") {
            response += "<tr><td>" + String(i+1) + "</td><td>" + systemLog[i] + "</td></tr>";
        }
    }
    
    response += "</table>";
    response += "<p><a href='/'>Go back</a></p>";
    response += "</body></html>";

    // Send the response as HTML
    server.send(200, "text/html", response);
}

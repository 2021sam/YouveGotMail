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
        // Add the /config route for configuration
    server.on("/config", HTTP_GET, [this]() { showConfigForm(); });
    server.on("/config", HTTP_POST, [this]() { handleConfig(); });
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



void WebEndpoints::showConfigForm() {
    // Load Wi-Fi credentials from "wifi" namespace
    preferences.begin("wifi", true);  // Open Wi-Fi namespace for reading
    String savedSSID = preferences.getString("wifi_ssid", "");  // Default to empty if not set
    String savedPassword = preferences.getString("wifi_password", "");  // Default to empty if not set
    preferences.end();  // Close Wi-Fi namespace

    // Load other settings from "config" namespace
    preferences.begin("config", true);  // Open Config namespace for reading
    String savedEmail1 = preferences.getString("recipient_email_1", "");  // Recipient Email 1
    String savedEmail2 = preferences.getString("recipient_email_2", "");  // Recipient Email 2
    String savedEmail3 = preferences.getString("recipient_email_3", "");  // Recipient Email 3
    String savedSMTP = preferences.getString("smtp_host", "smtp.gmail.com");  // SMTP Host
    int savedPort = preferences.getInt("smtp_port", 587);  // Default SMTP Port
    String savedHostname = preferences.getString("wifi_hostname", "Got_Mail");  // Wi-Fi Hostname
    String savedAuthorEmail = preferences.getString("author_email", "");  // Email for sending alerts
    String savedAuthorPassword = preferences.getString("author_password", "");  // Password for the sender email
    preferences.end();  // Close Config namespace

    // Generate the configuration form
    String response = "<html><head><style>"
                      "body { font-family: Arial, sans-serif; }"
                      "form { max-width: 400px; margin: 20px auto; }"
                      "input { width: 100%; padding: 8px; margin: 8px 0; }"
                      "button { padding: 10px; background-color: #2E8B57; color: white; border: none; }"
                      "</style></head><body>";
    response += "<h1>Device Configuration</h1>";
    response += "<form action='/config' method='POST'>";
    
    // Wi-Fi settings
    response += "<label for='ssid'>Wi-Fi SSID:</label>";
    response += "<input type='text' name='ssid' value='" + savedSSID + "'><br>";
    response += "<label for='password'>Wi-Fi Password:</label>";
    response += "<input type='password' name='password' value='" + savedPassword + "'><br>";
    
    // Email settings for sending alerts
    response += "<label for='recipient_email_1'>Recipient Email 1:</label>";
    response += "<input type='email' name='recipient_email_1' value='" + savedEmail1 + "'><br>";
    response += "<label for='recipient_email_2'>Recipient Email 2:</label>";
    response += "<input type='email' name='recipient_email_2' value='" + savedEmail2 + "'><br>";
    response += "<label for='recipient_email_3'>Recipient Email 3:</label>";
    response += "<input type='email' name='recipient_email_3' value='" + savedEmail3 + "'><br>";
    
    // Author Email (Sender's email)
    response += "<label for='author_email'>Sender Email:</label>";
    response += "<input type='email' name='author_email' value='" + savedAuthorEmail + "'><br>";
    
    // Author Password (App password for email)
    response += "<label for='author_password'>Sender Email Password:</label>";
    response += "<input type='password' name='author_password' value='" + savedAuthorPassword + "'><br>";
    
    // SMTP settings
    response += "<label for='smtp'>SMTP Host:</label>";
    response += "<input type='text' name='smtp' value='" + savedSMTP + "'><br>";
    response += "<label for='port'>SMTP Port:</label>";
    response += "<input type='number' name='port' value='" + String(savedPort) + "'><br>";

    // Wi-Fi Hostname
    response += "<label for='wifi_hostname'>Wi-Fi Hostname:</label>";
    response += "<input type='text' name='wifi_hostname' value='" + savedHostname + "'><br>";

    // Submit Button
    response += "<button type='submit'>Save Settings</button>";
    response += "</form></body></html>";

    server.send(200, "text/html", response);
}




// Handle form submission and save settings
void WebEndpoints::handleConfig() {
    if (server.hasArg("recipient_email_1") && server.hasArg("smtp_host") &&
        server.hasArg("smtp_port") && server.hasArg("author_email") &&
        server.hasArg("author_password") && server.hasArg("wifi_hostname")) {

        // Retrieve form data
        String recipientEmail1 = server.arg("recipient_email_1");
        String recipientEmail2 = server.arg("recipient_email_2");
        String recipientEmail3 = server.arg("recipient_email_3");
        String smtpHost = server.arg("smtp_host");
        int smtpPort = server.arg("smtp_port").toInt();
        String authorEmail = server.arg("author_email");
        String authorPassword = server.arg("author_password");
        String wifiHostname = server.arg("wifi_hostname");

        // Save data to Preferences
        preferences.begin("config", false);  // Open the "config" namespace
        preferences.putString("recipient_email_1", recipientEmail1);
        preferences.putString("recipient_email_2", recipientEmail2);
        preferences.putString("recipient_email_3", recipientEmail3);
        preferences.putString("smtp_host", smtpHost);
        preferences.putInt("smtp_port", smtpPort);
        preferences.putString("author_email", authorEmail);
        preferences.putString("author_password", authorPassword);
        preferences.putString("wifi_hostname", wifiHostname);
        preferences.end();  // Close Preferences

        Serial.println("Configuration saved!");
        server.send(200, "text/html", "<h1>Configuration Saved</h1><p><a href='/'>Go back</a></p>");
    } else {
        server.send(400, "text/html", "<h1>Invalid Input</h1><p>Please fill all required fields.</p><p><a href='/config'>Try again</a></p>");
    }
}
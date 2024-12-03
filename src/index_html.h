// IOT\index_html.h
#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char pageContent[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Mailbox Security</title>
  <style>
    body { font-family: Arial, sans-serif; }
    h1 { color: #2E8B57; }
    table { width: 50%; margin: 25px 0; border-collapse: collapse; }
    th, td { padding: 8px 12px; text-align: left; border: 1px solid #ddd; }
  </style>
</head>
<body>
  <h2>Links to Sensor Data:</h2>
  <ul>
    <li><a href="/device">Device Data</a></li>
    <li><a href="/distance">Distance Data</a></li>
    <li><a href="/lux">Light Level</a></li>
    <li><a href="/rssi">Wi-Fi RSSI</a></li>
    <li><a href="/alert">Send Test Email</a></li>
    <li><a href="/all">Auto Refresh All</a></li>
    <li><a href="/log">View Logs</a></li>  <!-- New link to view logs -->
    <li><a href="/config">Configure Settings</a></li>  <!-- New link for configuration -->
  </ul>
</body>
</html>
)";

#endif  // INDEX_HTML_H

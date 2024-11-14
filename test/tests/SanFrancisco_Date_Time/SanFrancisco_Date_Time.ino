#include <WiFi.h>
#include <time.h>

const char* ssid = "Your_SSID";       // Replace with your network SSID
const char* password = "Your_Password"; // Replace with your network password


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configure time for Pacific Standard Time (UTC-8)
  configTime(-8 * 3600, 0, "pool.ntp.org");

  // Get time
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  
  // Print the current time
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  Serial.print("Current time in SF Bay Area: ");
  Serial.println(buffer);
}

void loop() {
  // Your code here
}

# 1 "C:\\Users\\2021sam\\AppData\\Local\\Temp\\tmp7y00ejmo"
#include <Arduino.h>
# 1 "C:/Users/2021sam/apps/YouveGotMail/src/IOT_display.ino"

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
# 16 "C:/Users/2021sam/apps/YouveGotMail/src/IOT_display.ino"
#include "TFTDisplay.h"
#include "DistanceSensor.h"
#include "LightSensor.h"
#include "WebEndpoints.h"
#include "MailService.h"
#include "index_html.h"
#include "Alert.h"
#include "credentials.h"
#include "WiFiManager.h"


#define LED_PIN 21
TFTDisplay display;



#define DISTANCE_SENSOR_ADDRESS 0x29
#define LIGHT_SENSOR_ADDRESS 0x23




DistanceSensor distanceSensor(DISTANCE_SENSOR_ADDRESS);
LightSensor lightSensor(LIGHT_SENSOR_ADDRESS);

MailService mailService(AUTHOR_EMAIL, AUTHOR_PASSWORD, SMTP_HOST, SMTP_PORT, recipients, NUM_RECIPIENTS);



float previousDistance = 0;
unsigned long startTime;
const float distanceThresholdCM = 10.0;
const unsigned long firstEmailDelay = 5 * 60 * 1000;
const unsigned long emailCooldown = 10000;
unsigned long lastEmailTime = 0;


const int deliveryStartHour = 8;
const int deliveryEndHour = 17;

WebServer server(80);

WebEndpoints endpoints(server, &mailService, distanceSensor, lightSensor);




Alert alertSystem(distanceSensor, lightSensor, mailService, deliveryStartHour, deliveryEndHour);
void setup();
void loop();
#line 66 "C:/Users/2021sam/apps/YouveGotMail/src/IOT_display.ino"
void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);


    display.begin();
    display.showStatusMessage("Initializing...");



    String ipAddress = setup_WiFi(display);
    display.showStatusMessage(ipAddress);
    delay(2000);


    configTime(-8 * 3600, 0, "pool.ntp.org");


    Wire.begin();


    distanceSensor.begin();


    String statusMessage = "Distance Sensor online status: ";


    statusMessage += distanceSensor.isOnline() ? "Online" : "Offline";


    display.showStatusMessage(statusMessage);
    Serial.println(statusMessage);


    if (!lightSensor.begin()) {
        Serial.println("Light sensor initialization failed!");
        display.showStatusMessage("Light sensor initialization failed!");
    }
    else {
        Serial.println("Distance sensor initialized successfully!");

        Serial.println("Light sensor initialized successfully!");
    }


    server.begin();
    Serial.println("WebServer started");


    display.showStatusMessage("Setup Complete!");
    delay(2000);
    startTime = millis();
}


void loop() {

    server.handleClient();

    checkWiFiConnection(display);

    float currentDistance = distanceSensor.getDistance();
    float lux = lightSensor.getLightLevel();
    int rssi = WiFi.RSSI();


    String currentTime = alertSystem.getCurrentTime();

    Serial.printf("[%s] RSSI: %d, Distance: %.2f cm, Light Level: %.2f lux\n", currentTime.c_str(), rssi, currentDistance, lux);
# 143 "C:/Users/2021sam/apps/YouveGotMail/src/IOT_display.ino"
    display.showAllData(currentDistance, lux, rssi, currentTime);

    if (millis() - startTime > firstEmailDelay){
        String statusMessage = alertSystem.checkAndSendEmail();


        int threshold = 100;
        alertSystem.monitorLightSensor(threshold);

        if (statusMessage != "") {
        display.showStatusMessage(statusMessage);
        delay(10000);
        }
    }
    delay(2000);
}
#include <Wire.h>
#include <SparkFun_VL53L1X.h>
#define VL53L1X_I2C_ADDR 0x29
// Create an instance of the VL53L1X sensor
SFEVL53L1X distanceSensor;

// Function to check for I2C devices and return the address of the VL53L1X
uint8_t readI2CAddress() {
  Wire.beginTransmission(VL53L1X_I2C_ADDR);  // Default I2C address is 0x29
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("VL53L1X found at default address (0x29).");
    return VL53L1X_I2C_ADDR;
  } else {
    Serial.println("No response from default address (0x29), checking other addresses...");
    // Scan the I2C bus for any connected device
    for (uint8_t i = 1; i < 127; i++) {
      Wire.beginTransmission(i);
      error = Wire.endTransmission();
      if (error == 0) {
        Serial.print("Device found at address 0x");
        Serial.println(i, HEX);
        return i;
      }
    }
  }
  return 0;  // Return 0 if no device is found
}

void setup() {
    // Start serial communication for output
    Serial.begin(9600);
    
    // Initialize Wire library for I2C communication
    Wire.begin();
    
    // Call the function to find the I2C address
    uint8_t address = readI2CAddress();

    if (address != 0) {
        Serial.print("Sensor is located at I2C address 0x");
        Serial.println(address, HEX);
    } else {
        Serial.println("No I2C devices found!");
    }

// Initialize the VL53L1X sensor
    // bool initStatus = distanceSensor.begin();  // Get the return value of begin()
    int initStatus = distanceSensor.begin();  // Get the return value of begin()
    Serial.println("***");
    Serial.println(initStatus);
    Serial.println("***");
    // Print the result of the begin() method

    // Initialize the VL53L1X sensor
    if (initStatus != 0) {
        Serial.println("Sensor failed to begin. Please check wiring.");
        while (1);  // Stop here if sensor doesn't initialize
    }
    Serial.println("Sensor online!");
}

void loop() {
    // Start ranging (measure distance)
    distanceSensor.startRanging();

    // Wait until data is ready
    while (!distanceSensor.checkForDataReady()) {
        delay(1);  // Wait for data to be ready
    }

    // Get the distance in millimeters
    int distanceInMM = distanceSensor.getDistance();
    
    // Convert distance from mm to cm and print it
    float distanceInCM = distanceInMM / 10.0;
    Serial.print("Distance: ");
    Serial.print(distanceInCM);
    Serial.println(" cm");

    // Clear interrupt and stop ranging
    distanceSensor.clearInterrupt();
    distanceSensor.stopRanging();

    delay(1000);  // Wait a second before the next measurement
}

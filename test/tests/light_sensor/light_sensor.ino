#include <Wire.h>
#include <BH1750.h>

#define LIGHT_SENSOR_ADDRESS 0x23  // BH1750 default I2C address

// Create a BH1750 object
BH1750 lightMeter(LIGHT_SENSOR_ADDRESS);

// Function to initialize the light sensor with retry logic
bool initializeLightSensor() {
  int attempts = 3;
  while (attempts > 0) {
    if (lightMeter.begin()) {
      return true;  // Sensor initialized successfully
    }
    attempts--;
    Serial.println("Light sensor initialization failed. Retrying...");
    delay(500);  // Wait before retrying
  }
  return false;  // Sensor failed to initialize after 3 attempts
}

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  // Initialize the I2C communication
  Wire.begin();

  // Attempt to initialize the light sensor
  if (initializeLightSensor()) {
    Serial.println("Light sensor initialized successfully.");
    
    // Set the sensor to continuous high-resolution mode (you can change this to suit your needs)
    lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE);
  } else {
    Serial.println("Light sensor initialization failed after 3 attempts.");
  }
}

void loop() {
  // Read the light level in lux
  float lux = lightMeter.readLightLevel();
  
  // Check if the reading is valid
  if (lux < 0) {
    Serial.println("Error: Invalid light sensor reading.");
  } else {
    // Print the light level in lux
    Serial.print("Light Level: ");
    Serial.print(lux);
    Serial.println(" lux");
  }

  // Wait 1 second before taking the next reading
  delay(1000);
}

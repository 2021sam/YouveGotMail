# You've Got Mail

## Overview

"You've Got Mail" is a smart mail notification system using an ESP32 microcontroller and a VL53L1X distance sensor. This project alerts you via email when mail is delivered to your mailbox. It continuously monitors the distance from inside of the mailbox to the mailbox door, and if a significant change is detected, it sends an email notification.

## Features

- Monitors distance to the mailbox door.
- Sends email alerts upon detecting mail.
- Configurable distance thresholds.
- Wi-Fi connectivity for real-time notifications.

## Setup Instructions

### Hardware Requirements

- ESP32 development board
- VL53L1X distance sensor
- Jumper wires
- Breadboard (optional)

### Software Requirements

- Arduino IDE
- Required libraries:
  - SparkFun VL53L1X library
  - ESP Mail Client library
  - WiFi library (included with ESP32 core)

### Configuration

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/yourusername/YouveGotMail.git
   cd YouveGotMail

  Key points about the LilyGO T-Display S3 pinout:
    SDA pin: GPIO 17
    SCL pin: GPIO 18

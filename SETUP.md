This guide details how to configure the LilyGO T-Display S3 with the `TFT_eSPI` library in a PlatformIO project.

---

# LilyGO T-Display S3 Setup Guide for PlatformIO

This guide explains how to set up the **LilyGO T-Display S3** board with the `TFT_eSPI` library in a PlatformIO project. The T-Display S3 uses an ESP32-S3 microcontroller with an integrated ST7789 display, and this setup will help you utilize the display effectively.

## Prerequisites
- [PlatformIO](https://platformio.org/) installed in your preferred IDE (e.g., VSCode)
- Git

## Step 1: Clone the Repository
Start by cloning the **LilyGO T-Display S3** repository to access their customized `TFT_eSPI` library.

```bash
git clone https://github.com/Xinyuan-LilyGO/T-Display-S3.git
```

## Step 2: Copy the `TFT_eSPI` Library to Your Project
The LilyGO repository includes a custom version of the `TFT_eSPI` library configured for the T-Display S3. Copy this library to your local project.

1. Navigate to the cloned repository:

   ```bash
   cd T-Display-S3/lib
   ```

2. Copy the `TFT_eSPI` directory to your PlatformIO project’s `lib` folder:

   ```bash
   cp -r TFT_eSPI /path/to/your/platformio_project/lib/
   ```

## Step 3: Verify the Project Folder Structure
After copying, your project's folder structure should look like this:

```
your_project/
├── lib/
│   └── TFT_eSPI/
│       ├── User_Setup_Select.h
│       ├── User_Setup.h
│       ├── (other files)
├── src/
│   └── main.cpp
└── platformio.ini
```

## Step 4: Update `platformio.ini`
Make sure your `platformio.ini` file includes the necessary configurations for the T-Display S3:

```ini
[env:lilygo_t_display_s3]
platform = espressif32
board = lilygo-t-display-s3
framework = arduino
upload_port = COM6
upload_speed = 115200
monitor_speed = 115200
lib_deps =
    sparkfun/SparkFun VL53L5CX Arduino Library@^1.0.3
    sparkfun/SparkFun VL53L1X 4m Laser Distance Sensor@^1.2.12
    claws/BH1750@^1.3.0
    mobizt/ESP Mail Client@^3.4.22
```

**Note:** You don’t need to include `TFT_eSPI` in the `lib_deps` section since it’s now a local library.

## Step 5: Customize `User_Setup.h` (Optional)
If needed, modify the `User_Setup.h` file located in `lib/TFT_eSPI/` to adjust display settings such as pin configurations:

```cpp
#define ST7789_DRIVER
#define TFT_WIDTH  320
#define TFT_HEIGHT 170

#define TFT_MISO -1
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10  // Chip select control pin
#define TFT_DC   13  // Data Command control pin
#define TFT_RST  -1  // (or connect to +3.3V)
#define TFT_BL   38  // Backlight control pin

#define TFT_BACKLIGHT_ON HIGH

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
```

## Step 6: Test the Display
Create a simple sketch in `src/main.cpp` to verify that the display is working:

```cpp
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Hello, LilyGO T-Display S3!", 10, 10);
}

void loop() {
}
```

## Step 7: Upload and Monitor
Use PlatformIO to upload the code to your board and monitor the serial output:

```bash
pio run --target upload
pio device monitor
```

## Additional Notes
- This setup ensures that your `TFT_eSPI` library configuration is specific to your project and won’t be overwritten by PlatformIO updates.
- Refer to the [LilyGO T-Display S3 documentation](https://github.com/Xinyuan-LilyGO/T-Display-S3) for more examples and troubleshooting.

---

This guide can be included directly in your GitHub repository for others to follow. Let me know if you need further customization!
#define BTN_STOP_ALARM 0
#define LED_PIN 21 // LED connected to pin 21

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_STOP_ALARM, INPUT);
  pinMode(LED_PIN, OUTPUT);

  timerSemaphore = xSemaphoreCreateBinary();
  
  // Set timer to trigger every 100 ms
  timer = timerBegin(1000); // 100 ms in microseconds
  timerAttachInterrupt(timer, &onTimer);
  
  // Set alarm to call onTimer function every 100 ms with autoreload
  timerAlarm(timer, 100, true, 0); // Autoreload
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    uint32_t isrCount, isrTime;
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
    
    // Control LED based on the interrupt count
    //  Seeed Xiao - low turning on the LED and high turning it off:
    if (isrCount < 2) { // First 3 interrupts for 300 ms ON
      digitalWrite(LED_PIN, HIGH);
    } else if (isrCount < 10) { // Next 7 interrupts for 700 ms OFF
      digitalWrite(LED_PIN, LOW);
    } else { // Reset counter after 10 interrupts (1 second)
      isrCounter = 0; // Reset the counter for the next second cycle
    }
    
    Serial.print("onTimer no. ");
    Serial.print(isrCount);
    Serial.print(" at ");
    Serial.print(isrTime);
    Serial.println(" ms");
  }

  // If button is pressed, stop the timer
  if (digitalRead(BTN_STOP_ALARM) == LOW) {
    if (timer) {
      timerEnd(timer);
      timer = NULL;
    }
  }
}

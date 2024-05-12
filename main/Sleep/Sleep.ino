#include <esp_sleep.h>

const int interruptPin = 6;

void setup() {
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, HIGH);
  
  // Set up wake up using timer for 6 seconds
  esp_sleep_enable_timer_wakeup(10 * 1000000);  // 6 seconds in microseconds

  // Set up wake up using external interrupt on specified pin
  esp_sleep_enable_ext1_wakeup(BIT(6), ESP_EXT1_WAKEUP_ANY_HIGH);
}

void loop() {
  // Enter deep sleep mode
  
  esp_deep_sleep_start();
}

void wakeUp() {
  // Your wake-up code here
  Serial.println("Awake!");
}

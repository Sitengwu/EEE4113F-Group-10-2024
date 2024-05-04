/*
  BlinkRGB

  Demonstrates usage of onboard RGB LED on some ESP dev boards.

  Calling digitalWrite(RGB_BUILTIN, HIGH) will use hidden RGB driver.
    
  RGBLedWrite demonstrates controll of each channel:
  void neopixelWrite(uint8_t pin, uint8_t red_val, uint8_t green_val, uint8_t blue_val)

  WARNING: After using digitalWrite to drive RGB LED it will be impossible to drive the same pin
    with normal HIGH/LOW level
*/
//#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)

// the setup function runs once when you press reset or power the board

void setup() {
}

// the loop function runs over and over again forever
void loop() {
  for(int i = 0; i < 256; i++)
  {
    neopixelWrite(48,i,255-i,0);
    delay(20);
  }
  for(int i = 0; i < 256; i++)
  {
    neopixelWrite(48,255-i,0,i);
    delay(20);
  }
  for(int i = 0; i < 256; i++)
  {
    neopixelWrite(48,0,i,255-i);
    delay(20);
  }
}

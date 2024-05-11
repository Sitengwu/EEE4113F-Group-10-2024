boolean toggle = false;

void IRAM_ATTR handleInterrupt() {
  // Your interrupt handling code here
  if (toggle)
  {
    neopixelWrite(48, 65,65,0);  // Turn on LED
    toggle = false;
    delay(500);
  }
  else
  {
    neopixelWrite(48, 0,0,0);  // Turn on LED
    toggle = true;
    delay(500);
  }
}

void setup() {

  Serial.begin(115200);
  neopixelWrite(48, 0,0,0);

  pinMode(6, INPUT_PULLDOWN);

  // Attach interrupt to the GPIO pin
  attachInterrupt(digitalPinToInterrupt(6), handleInterrupt, RISING);
}

void loop() {
}
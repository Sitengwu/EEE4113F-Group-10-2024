void setup() {
  pinMode(6, INPUT);  // Set input pin
  Serial.begin(115200);
  neopixelWrite(48, 0,0,0);
}

void loop() {
  // Read the state of the input pin
  int inputState = digitalRead(6);

  // If input pin is high (i.e., button pressed)
  if (inputState == 1) {
    neopixelWrite(48, 65,65,0);  // Turn on LED
  } 
}

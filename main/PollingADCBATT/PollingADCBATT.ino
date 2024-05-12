const int adcPin = 7; // Example ADC pin

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Set ADC resolution to 12 bits
}

void loop() {
  // Read the ADC value
  int adcValue = analogRead(adcPin);

  // Convert ADC value to voltage (in volts) using internal reference voltage
  float voltage = ((float)adcValue / 4095.0) * 2.6; // 1100mV is the internal reference voltage (1.1V) 

  // Print the results
  Serial.print("Batt %: ");
  Serial.println((voltage/3.0)*100, 2); // Print voltage with 3 decimal places

  delay(1000); // Delay for readability
}
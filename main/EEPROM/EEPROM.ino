#include <EEPROM.h>

void setup() {
  Serial.begin(115200);

  // Initialize EEPROM with the size of the data you want to save
  EEPROM.begin(sizeof(bool) + sizeof(long));

  // Read the boolean value from EEPROM
  bool boolValue;
  EEPROM.get(0, boolValue);
  Serial.print("Boolean value read from EEPROM: ");
  Serial.println(boolValue ? "true" : "false");

  // Read the long integer value from EEPROM
  long longValue;
  EEPROM.get(sizeof(bool), longValue);
  Serial.print("Long integer value read from EEPROM: ");
  Serial.println(longValue);

  // Increment the long integer value and write it back to EEPROM
  longValue++;
  EEPROM.put(sizeof(bool), longValue);
  EEPROM.commit();
  Serial.print("Long integer value incremented and saved to EEPROM: ");
  Serial.println(longValue);
}

void loop() {
  // Your main code here
}

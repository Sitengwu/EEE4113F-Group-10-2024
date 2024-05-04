#include <BluetoothSerial.h>
#include <SD.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("Card mounted");

  // Initialize Bluetooth
  SerialBT.begin("ESP32_BT"); // Name of your ESP32 Bluetooth device

  Serial.println("ESP32 Initialized");
}

void loop() {
  // Check if there is any data available to send over Bluetooth
  if (SerialBT.available()) {
    // Open the file to be sent
    File file = SD.open("/Test.txt");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    // Read data from the file and send it over Bluetooth
    while (file.available()) {
      SerialBT.write(file.read());
    }

    // Close the file
    file.close();

    // Wait for a short period before sending the next chunk of data
    delay(100);
  }
}


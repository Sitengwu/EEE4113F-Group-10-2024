#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <SD.h>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

File file;
const char* fileName = "/Test.txt";
int fileSize;
const int chunkSize = 20;  // Adjust this based on your requirements

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device connected to BLE");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Device disconnected from BLE");
    }
};

void setup() {
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("Card mounted");

  // Open the file
  file = SD.open(fileName);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Get the file size
  fileSize = file.size();

  // Initialize BLE
  BLEDevice::init("ESP32_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService("bb486657-341b-44f1-9594-41136946a471");
  pCharacteristic = pService->createCharacteristic("293a7b0d-7067-4d0c-a60e-eec6b41ad45e", BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();
  
  Serial.println("BLE server started");
}

void loop() {
  // Read data from the file and send it over BLE in chunks
  uint8_t buf[chunkSize];
  int bytesRead = file.read(buf, chunkSize);
  if (bytesRead > 0) {
    pCharacteristic->setValue(buf, bytesRead);
    pCharacteristic->notify();
    delay(100);  // Adjust delay based on your requirements
  } else {
    // End of file, close the file and stop BLE
    file.close();
    pServer->getAdvertising()->stop();
    Serial.println("File transfer complete");
    while (1) {
      delay(1000);
    }
  }
}


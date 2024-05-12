#include "HX711.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <string>
#include <cstdio>
#include <esp_sleep.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//RFID
String ID;
const String Birds[] = { "33000CE379A5", "33000D51AAC5", "33000E15674F" };
const int RFIDpins[] = { 15, 16, 17 };
char c;

//Scale
boolean birdON;
long offset; 
const int LOADCELL_DOUT_PIN = 8;
const int LOADCELL_SCK_PIN = 9;
HX711 scale;

//WiFi
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 30000; // 1 minute in milliseconds
const char* filepath = "/Data.txt";
boolean rcd = false;
volatile bool wifiInt = false;
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";
IPAddress localIP(192, 168, 1, 1); // Manually set IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
AsyncWebServer server(80);

void IRAM_ATTR handleInterrupt() {
  // Your interrupt handling code here
  if(WiFi.getMode() != WIFI_AP)
  {
    Serial.println("Remote triggered...");
    wifiInt = true;
  }
}

void setup() 
{
  Serial.begin(115200);

  // Setting up non-volatile memory
  EEPROM.begin(sizeof(bool) + sizeof(long));

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  // Setting up ADC
  analogReadResolution(12);

  // Setting up scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(122.2109);

  EEPROM.get(sizeof(bool), offset);
  EEPROM.get(0, birdON);

  Serial.println(birdON ? "true" : "false");

  if (offset > 100000)
  {
    scale.set_offset(offset);
  }
  else
  {
    scale.tare();
    offset = scale.get_offset();
    EEPROM.put(sizeof(bool), offset);
    EEPROM.commit();
  }

  // Setting up RFID reader
  Serial1.setRxBufferSize(129);
  Serial1.begin(9600, SERIAL_8N1, 4, 5);

  for (int i : RFIDpins) {
    pinMode(i, OUTPUT);
  }

  // Priming WIFI
  WiFi.mode(WIFI_OFF);

  // Attach interrupt to the GPIO pin
  attachInterrupt(digitalPinToInterrupt(6), handleInterrupt, HIGH);

  // Set up wake up using timer
  esp_sleep_enable_timer_wakeup(10 * 1000000);

  // Set up wake up using external interrupt on specified pin
  esp_sleep_enable_ext1_wakeup(BIT(6), ESP_EXT1_WAKEUP_ANY_HIGH);
}

void loop() 
{
  if (rcd)
  {
    Serial.print("Shutting down WiFi");
    server.end();
    WiFi.softAPdisconnect(true);
    rcd = false;
    esp_deep_sleep_start();
  }

  if (wifiInt)
  { 
    Serial.println("Starting Access Point");
    WiFi.softAPConfig(localIP, gateway, subnet);
    WiFi.softAP(ssid, password);
    
    server.on("/download", HTTP_GET, serveFile);
    server.on("/received", HTTP_GET, received);
    server.begin();
    Serial.println("Access Point Started");
    previousMillis = millis();
    wifiInt = false;
    delay(5000);
  }

  while(WiFi.softAPgetStationNum()==0 && WiFi.getMode() == WIFI_MODE_AP)
  {
    currentMillis = millis();
    Serial.println("Waiting for connection...");
    delay(1000);
    if (currentMillis - previousMillis >= interval) 
    {
      // Update the previousMillis for the next interval
      currentMillis = 0;
      Serial.println("Connection Timeout: Shutting down WiFi");
      // Turn off WiFi
      server.end();
      WiFi.softAPdisconnect(true);
      delay(5000);
    }
  }
  scale.power_up();
  if (scale.get_units(1) > 2200) 
  {
    if(!birdON)
    {
      EEPROM.put(0, true);
      EEPROM.commit();

      Serial.println("Weight detected...");

      for (int pin : RFIDpins) 
      {
        digitalWrite(pin, HIGH);
        
        if (readRFID()) 
        {
          if (SD.exists("/Data.txt")) 
          {
            File file = SD.open("/Data.txt", FILE_APPEND);
            file.print(scale.get_units(20));
            scale.power_down();
            file.print("," + ID + ",");
            file.println(pollBatt(7));
            Serial.println("Data added");
            file.close();
          }
          else 
          {
            File file = SD.open("/Data.txt", FILE_WRITE);
            file.print(scale.get_units(20));
            scale.power_down();
            file.print("," + ID + ",");
            file.println(pollBatt(7));
            Serial.println("Data added");
            file.close();
          }
          break;
        }
        digitalWrite(pin, LOW);
      }
      ID = "";
    }
  }
  else
  {
    EEPROM.put(0, false);
    EEPROM.commit();
  }
  scale.power_down();

  if(WiFi.getMode() == WIFI_OFF)
  {
    esp_deep_sleep_start();
  }
}

void clearSerialBuffer() {
  while (Serial1.available() > 0) {
    char _ = Serial1.read();  // Read and discard each character
  }
}

bool isAlphanumeric(String str) {
  // Iterate through each character of the string
  for (char c : str) {
    // Check if the character is not a digit or a letter
    if (!isalnum(c) || c == '\0') {
      return false;  // Return false if it's not alphanumeric
    }
  }
  return true;  // Return true if all characters are alphanumeric
}

uint8_t check() {
  ID = ID.substring(1, 13);
  if (isAlphanumeric(ID)) {
    for (String s : Birds) {
      if (ID == s) {
        return 0;
      }
    }
    return 1;
  }
  return 2;
}

bool readRFID() {
  clearSerialBuffer();
  delay(300);
  while (Serial1.available() > 0) {
    c = Serial1.read();
    ID += c;
    if (ID.length() > 20) {
      switch (check()) {
        case 0:
          Serial.println("Bird Detected - ID: " + ID);
          clearSerialBuffer();
          return true;
        case 1:
          Serial.println("Not a Bird - ID: " + ID);
          clearSerialBuffer();
          return false;
      }
      clearSerialBuffer();
      return false;
    }
  }
}

void received(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Turning WiFi Off");
  rcd = true;
  SD.remove(filepath);
  File file = SD.open(filepath, FILE_WRITE);
  file.close();
}

void serveFile(AsyncWebServerRequest *request) {
  File file = SD.open(filepath); // Change the path to your file
  if (file) {
    request->send(SD, filepath, "text/plain");
    file.close();
  } else {
    request->send(404, "text/plain", "File not found");
  }
}

float pollBatt(u_int8_t pin)
{
  int adcValue = analogRead(pin);
  float voltage = (float)adcValue * 3 / 4095.0; // 4095 is the maximum value for a 12-bit ADC
  return (voltage / 3) * 100.0;
}
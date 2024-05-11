#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>

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

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  // Attach interrupt to the GPIO pin
  attachInterrupt(digitalPinToInterrupt(6), handleInterrupt, HIGH);
  
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  // Set up wake up using timer for 6 seconds
  esp_sleep_enable_timer_wakeup(10 * 1000000);  // 6 seconds in microseconds

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
  if(WiFi.getMode() == WIFI_OFF)
  {
    esp_deep_sleep_start();
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
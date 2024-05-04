#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>

const char* filepath = "/Test.txt";
boolean rcd = false;
volatile bool wifiInt = false;

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

IPAddress localIP(192, 168, 1, 1); // Manually set IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  
  pinMode(interruptPin, INPUT_PULLDOWN);

  // Attach interrupt to the GPIO pin
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING);
  
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
}

void loop() 
{
  if (rcd)
  {
    WiFi.softAPdisconnect(true);
    rcd = false;
  }

  if (wifiInt)
  { 
    Serial.print("Setting AP (Access Point)...");
    WiFi.softAPConfig(localIP, gateway, subnet);
    WiFi.softAP(ssid, password);
    
    server.on("/download", HTTP_GET, serveFile);
    server.on("/received", HTTP_GET, received);
    server.begin();

    wifiInt = false;
  }

}

void IRAM_ATTR handleInterrupt() {
  // Your interrupt handling code here
  if(WiFi.softAPgetHostname() != "")
  {
    wifiInt = true;
  }
}

void received(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Shutting down...");
  rcd = true;
  SD.remove(filepath);
  File file = SD.open(filepath, FILE_WRITE);
  file.print("Hello World!\n");
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
#include <WiFi.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

void setup() {
  // put your setup code here, to run once:
  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP(ssid, password);
}

void loop() {
  // put your main code here, to run repeatedly:

}

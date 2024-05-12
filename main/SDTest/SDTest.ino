#include "FS.h"
#include "SD.h"
#include "SPI.h"

void setup() {
  Serial.begin(115200);
  SD.begin(10);

  File f = SD.open("/Test.txt", FILE_WRITE);
  f.println("Hello World!");
  f.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}

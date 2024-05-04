String text;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 4, 5);
  // Serial.flush(); // wait Serial FIFO to be empty and then spend almost no time processing it
  // Serial1.setRxFIFOFull(20); // testing diferent result based on FIFO Full setup
  // Serial1.onReceive(onReceiveFunction, onlyOnTimeOut); // sets a RX callback function for Serial 1
}
char c;

void loop()
{
  while (Serial1.available() > 0) 
  {
    c = Serial1.read();
    text += c;
    if (text.length() > 20)
    {
      if (check())
      {
        clearSerialBuffer();
        break;
      }  
    }
  }
  delay(1000);
}

void clearSerialBuffer() {
  while (Serial1.available() > 0) {
    char _ = Serial1.read(); // Read and discard each character
  }
}

bool isAlphanumeric(String str) {
  // Iterate through each character of the string
  for (int i = 0; str[i] != '\0'; i++) {
    // Check if the character is not a digit or a letter
    if (!isalnum(str[i])) {
      return false; // Return false if it's not alphanumeric
    }
  }
  return true; // Return true if all characters are alphanumeric
}

boolean check()
{
  text = text.substring(1, 11);

  if(isAlphanumeric(text))
  {
    Serial.println("Card ID : " + text);
    text = "";
    delay(100);
    return true;
  }
  text = "";
  delay(100);  
  return false;
}
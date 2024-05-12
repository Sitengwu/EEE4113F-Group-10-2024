String ID;
String Birds[] = {"33000CE379A5", "33000D51AAC5", "33000E15674F"};

void setup()
{
  Serial.begin(115200);
  Serial1.setRxBufferSize(129);
  Serial1.begin(9600, SERIAL_8N1, 4, 5);
}
char c;

void loop()
{
  readRFID();
  ID = "";
}

void clearSerialBuffer() {
  while (Serial1.available() > 0) {
    char _ = Serial1.read(); // Read and discard each character
  }
}

bool isAlphanumeric(String str) {
  // Iterate through each character of the string
  for (char c: str) {
    // Check if the character is not a digit or a letter
    if (!isalnum(c) || c == '\0') {
      return false; // Return false if it's not alphanumeric
    }
  }
  return true; // Return true if all characters are alphanumeric
}

bool readRFID()
{
  clearSerialBuffer();
  delay(300);
  while (Serial1.available() > 0) 
  {
    c = Serial1.read();
    ID += c;
    if (ID.length() > 15)
    {
      switch (check()) 
      {
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

uint8_t check()
{
  ID = ID.substring(1, 13);
  if(isAlphanumeric(ID))
  {
    for(String s: Birds)
    {
      if(ID == s)
      {
        return 0;
      }
    }
    return 1;
  }
  return 2;
}
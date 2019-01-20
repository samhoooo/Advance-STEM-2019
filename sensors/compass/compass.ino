char valueBytes[8];
int degree = 0;
int counter = 0;
byte value = 0;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  int direction = getCompass();
  delay(500);
}

int getCompass() {
  
  value = 0;

  Serial.write(0x31);
  if (Serial.available()) {
    Serial.read();
  }
  while (value == 0) {
    if (Serial.available()) {
      valueBytes[counter] = Serial.read();
      counter = (counter + 1) % 8;
      if (counter == 0) {
        degree = (valueBytes[2] - 48) * 100 + (valueBytes[3] - 48) * 10 + (valueBytes[4] - 48);
        value = 1;
      }
    }
  }
  return degree;
}

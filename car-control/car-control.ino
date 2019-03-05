#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
IPAddress host;
char devID[3] = "";

WiFiEspUDP Udp;

void ctrlforward(byte speed, int wait = 0, byte acceptance = 0, int cycle = 1);
void ctrlbackward(byte speed, int wait = 0, byte acceptance = 0, int cycle = 1);
void ctrlright(byte speed, int wait = 0, byte acceptance = 0, int cycle = 1);
void ctrlleft(byte speed, int wait = 0, byte acceptance = 0, int cycle = 1);

// already a 7 millisecond delay from 9600 baud communication
// see https://www.microchip.com/forums/m405110.aspx
const byte ENABLE = 13;
const byte SAIN1 = 11; // motor A
const byte SAIN2 = 12;
const byte SBIN1 = 10; // motor B
const byte SBIN2 = 8;
const byte SCIN1 = 3;  // motor C
const byte SCIN2 = 7;
const byte SDIN1 = 2;  // motor D
const byte SDIN2 = 4;




/*Notes and Warnings
  The PWM outputs generated on pins 5 and 6 will have higher-than-expected duty cycles.
  This is because of interactions with the millis() and delay() functions,
  which share the same internal timer used to generate those PWM outputs.
  This will be noticed mostly on low duty-cycle settings (e.g. 0 - 10)
  and may result in a value of 0 not fully turning off the output on pins 5 and 6.
*/

// PWM values
const byte pwm0 = 0; //PWM 0%

// used to getCompass value
char valueBytes[8];
int degree = 0;
int counter = 0;
byte value = 0;


void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial3.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial3);
  // initialize serial for GY26
  Serial1.begin(9600);
  
  
    // Print WiFi MAC address
  printMacAddress();
  Serial.println("Performing network scan");
  listNetworks();
  WiFi.begin("HIDDENWIFI", "HonestyIsTheBestPolicy");



  Serial.print("UDP broadcast on 9876");
  Udp.begin(9876);
  while (true) {
    Serial.print(".");
    delay(400);
    if (Udp.parsePacket()) {

      Serial.println();
      Serial.print("IP found: ");
      host = Udp.remoteIP();
      Serial.println(host);
      break;
    }
  }
  

  // Initialize digital pins
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN2, OUTPUT);
  pinMode(SBIN2, OUTPUT);
  pinMode(SCIN2, OUTPUT);
  pinMode(SDIN2, OUTPUT);

  start();
}

void loop() {
  adjustCarToNorth();
  while (true) {
    ctrlforward(50, 3, 10, 50);
    ctrlright(50, 3, 10, 50);
    ctrlbackward(50, 3, 10, 50);
    ctrlleft(50, 3, 10, 50);

  }

}


byte clip255(int input) {
  return constrain(input, 0, 255);
}
int compasstranslate(int compass, int translate) {
  return (compass + translate) % 360;

}
int degerr(int input) {
  if (input < 180) {
    return input;
  } else {
    return 360 - input;
  }
}
void adjustCarToNorth() {
  int compassDirection = getCompass();
  int compassDiff;
  bool clockwise;
  int targetspeed;
  while (compassDirection != 0) {
    compassDirection = getCompass();
    compassDiff = compassDirection;
    clockwise = false;
    if (compassDirection > 180) {
      compassDiff = 360 - compassDirection;
      clockwise = true;
    }
    targetspeed = 120;
    if (compassDiff < 70) {
      targetspeed = map(compassDiff, 0, 70, 20, 80);
    }
    if (clockwise) {
      selfRotationAntiClockwise(targetspeed);
    } else {
      selfRotationClockwise(targetspeed);
    }
  }
}
void move(char motor, String direction, byte speed) {
  int modeAD;
  int modeBC;
  int pwmAD;
  int pwmBC;
  if (direction.equals("clockwise")) {
    modeAD = HIGH;
    modeBC = LOW;
    pwmAD = 255 - speed;
    pwmBC = 0 + speed;
  } else if (direction.equals("anti-clockwise")) {
    modeAD = LOW;
    modeBC = HIGH;
    pwmAD = 0 + speed;
    pwmBC = 255 - speed;
  } else {
    return;
  }

  switch (motor) {
    case 'A':
      digitalWrite(SAIN2, modeAD);
      analogWrite(SAIN1, pwmAD);
      break;
    case 'D':
      digitalWrite(SDIN2, modeAD);
      analogWrite(SDIN1, pwmAD);
      break;
    case 'B':
      digitalWrite(SBIN2, modeBC);
      analogWrite(SBIN1, pwmBC);
      break;
    case 'C':
      digitalWrite(SCIN2, modeBC);
      analogWrite(SCIN1, pwmBC);
      break;
    default:
      break;
  }
}



void stop() {
  digitalWrite(ENABLE, LOW);
}

void start() {
  // Initialize analog pins
  analogWrite(SAIN1, pwm0);
  analogWrite(SBIN1, pwm0);
  analogWrite(SCIN1, pwm0);
  analogWrite(SDIN1, pwm0);

  digitalWrite(ENABLE, HIGH);
}

void forward(byte speed) {
  move('A', "clockwise", speed);
  move('B', "clockwise", speed);
  move('C', "clockwise", speed);
  move('D', "clockwise", speed);
}
void ctrlforward(byte speed, int wait, byte acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * (acceptance - degerr(compass)) / acceptance);

    if (compass < 180) {
      move('A', "clockwise", speed);
      move('B', "clockwise", speed);
      move('C', "clockwise", correction);
      move('D', "clockwise", correction);
    } else {
      move('A', "clockwise", correction);
      move('B', "clockwise", correction);
      move('C', "clockwise", speed);
      move('D', "clockwise", speed);
    }
    delay(wait);

  }

}
void backward(byte speed) {
  move('A', "anti-clockwise", speed);
  move('B', "anti-clockwise", speed);
  move('C', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
}


void ctrlbackward(byte speed, int wait, byte acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * (acceptance - degerr(compass)) / acceptance);

    if (compass > 180) {
      move('A', "anti-clockwise", speed);
      move('B', "anti-clockwise", speed);
      move('C', "anti-clockwise", correction);
      move('D', "anti-clockwise", correction);
    } else {
      move('A', "anti-clockwise", correction);
      move('B', "anti-clockwise", correction);
      move('C', "anti-clockwise", speed);
      move('D', "anti-clockwise", speed);
    }
    delay(wait);

  }

}

//
void right(byte speed) {
  move('A', "anti-clockwise", speed);
  move('B', "clockwise", speed);
  move('C', "anti-clockwise", speed);
  move('D', "clockwise", speed);
}
void ctrlright(byte speed, int wait, byte acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 180) {
      correction = clip255(speed * (acceptance - (360 - compass)) / acceptance);
    } else {
      correction = clip255(speed * (acceptance - compass) / acceptance);
    }
    if (compass < 180) {
      move('A', "anti-clockwise", correction);
      move('B', "clockwise", speed);
      move('C', "anti-clockwise", speed);
      move('D', "clockwise", correction);
    } else {
      move('A', "anti-clockwise", speed);
      move('B', "clockwise", correction);
      move('C', "anti-clockwise", correction);
      move('D', "clockwise", speed);
    }
    delay(wait);

  }

}
void left(byte speed) {
  move('B', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
  move('A', "clockwise", speed);
  move('C', "clockwise", speed);
}

void ctrlleft(byte speed, int wait, byte acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 180) {
      correction = clip255(speed * (acceptance - (360 - compass)) / acceptance);
    } else {
      correction = clip255(speed * (acceptance - compass) / acceptance);
    }
    if (compass < 180) {
      move('A', "clockwise", speed);
      move('B', "anti-clockwise", correction);
      move('C', "clockwise", correction);
      move('D', "anti-clockwise", speed);
    } else {
      move('A', "clockwise", correction);
      move('B', "anti-clockwise", speed);
      move('C', "clockwise", speed);
      move('D', "anti-clockwise", correction);
    }
    delay(wait);

  }

}


void selfRotationClockwise(byte speed) {
  move('C', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
  move('A', "clockwise", speed);
  move('B', "clockwise", speed);
}

void selfRotationAntiClockwise(byte speed) {
  move('A', "anti-clockwise", speed);
  move('B', "anti-clockwise", speed);
  move('C', "clockwise", speed);
  move('D', "clockwise", speed);
}

int getCompass() {

  value = 0;

  Serial1.write(0x31);
  if (Serial1.available()) {
    Serial1.read();
  }
  while (value == 0) {
    if (Serial1.available()) {
      valueBytes[counter] = Serial1.read();
      counter = (counter + 1) % 8;
      if (counter == 0) {
        degree = (valueBytes[2] - 48) * 100 + (valueBytes[3] - 48) * 10 + (valueBytes[4] - 48);
        value = 1;
      }
    }
  }
  return degree;
}


void printMacAddress()
{
  // get your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  String(mac[0], HEX).toCharArray(devID, 3);
  Serial.print("Device id: ");
  Serial.println(devID);
  // print MAC address
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);

}

void listNetworks()
{
  // scan for nearby networks
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen
  Serial.print("Number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.print("WEP");
      break;
    case ENC_TYPE_WPA_PSK:
      Serial.print("WPA_PSK");
      break;
    case ENC_TYPE_WPA2_PSK:
      Serial.print("WPA2_PSK");
      break;
    case ENC_TYPE_WPA_WPA2_PSK:
      Serial.print("WPA_WPA2_PSK");
      break;
    case ENC_TYPE_NONE:
      Serial.print("None");
      break;
  }
  Serial.println();
}

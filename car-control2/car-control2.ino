
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
IPAddress host;
String devID;
#define DIRECTION_FW 'W'
#define DIRECTION_LT 'A'
#define DIRECTION_BW 'S'
#define DIRECTION_RT 'D'




WiFiEspUDP Udp;
WiFiEspClient client;

void ctrlforward(int speed, int wait = 0, int acceptance = 0, int cycle = 1);
void ctrlbackward(int speed, int wait = 0, int acceptance = 0, int cycle = 1);
void ctrlright(int speed, int wait = 0, int acceptance = 0, int cycle = 1);
void ctrlleft(int speed, int wait = 0, int acceptance = 0, int cycle = 1);
void adjustCarToNorth(int angle = 0);

// already a 7 millisecond delay from 9600 baud communication
// actually a 21 ms delay built in in actual testing
// see https://www.microchip.com/forums/m405110.aspx
const int ENABLE = 13;
const int SAIN1 = 11; // motor A
const int SAIN2 = 12;
const int SBIN1 = 10; // motor B
const int SBIN2 = 8;
const int SCIN1 = 3;  // motor C
const int SCIN2 = 7;
const int SDIN1 = 2;  // motor D
const int SDIN2 = 4;




//Notes and Warnings
//The PWM outputs generated on pins 5 and 6 will have higher-than-expected duty cycles.
//This is because of interactions with the millis() and delay() functions,
//which share the same internal timer used to generate those PWM outputs.
//This will be noticed mostly on low duty-cycle settings (e.g. 0 - 10)
//and may result in a value of 0 not fully turning off the output on pins 5 and 6.





void setup() {
  // initialize serial for debugging
  Serial.begin(2000000);
  // initialize serial for ESP module
  Serial3.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial3);
  // initialize serial for GY26
  Serial1.begin(9600);


  // Print WiFi MAC address
  printMacAddress();
  //  Serial.println("Performing network scan");
  //  listNetworks();
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
    senddata(devID, DIRECTION_FW);
    ctrlforward(100, 3, 100, 20);
    senddata(devID, DIRECTION_RT);
    ctrlright(50, 3, 100, 50);
    senddata(devID, DIRECTION_BW);
    ctrlbackward(100, 3, 100, 20);
    senddata(devID, DIRECTION_LT);
    ctrlleft(50, 3, 100, 50);

  }

}


int clip255(int input) {
  return constrain(input, -255, 255);
}
int compasstranslate(int compass, int translate) {
  return (compass + translate) % 3600;

}
int degerr(int input) {
  if (input < 1800) {
    return input;
  } else {
    return 3600 - input;
  }
}
void adjustCarToNorth(int angle) {

  int trial = 0;
  int compassDirection = compasstranslate(getCompass(), angle);
  int compassDiff;
  bool clockwise;
  int targetspeed;
  while (compassDirection != 0) {

    compassDirection = compasstranslate(getCompass(), angle);

    compassDiff = degerr(compassDirection);
    //    clockwise = false;
    clockwise = compassDirection > 1800;
    //    if (compassDirection > 1800) {
    //      compassDiff = 3600 - compassDirection;
    //      clockwise = true;
    //    }
    targetspeed = 130;
    if (compassDiff < 500) {
      targetspeed = map(compassDiff, 0, 500, 30, 130);
    }
    trial++;

    if (trial > 150) {
      selfRotationAntiClockwise(160);
      compassDirection = 1;
      delay(75);
      trial = 0;
    }
    targetspeed = min(targetspeed, trial * 5);
    if (clockwise) {
      selfRotationAntiClockwise(targetspeed);
    } else {
      selfRotationClockwise(targetspeed);
    }
  }
}

void move(char motor, String direction, int speed) {
  if (speed < 0) {
    if (direction.equals("clockwise")) {
      move2(motor, "anti-clockwise", -speed);
    } else if (direction.equals("anti-clockwise")) {
      move2(motor, "clockwise", -speed);
    } else {
      return;
    }
  } else {
    move2(motor, direction, speed);
  }
}


void move2(char motor, String direction, int speed) {
  boolean modeAD;
  boolean modeBC;
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
  analogWrite(SAIN1, 0);
  analogWrite(SBIN1, 0);
  analogWrite(SCIN1, 0);
  analogWrite(SDIN1, 0);

  digitalWrite(ENABLE, HIGH);
}

void forward(int speed) {
  move('A', "clockwise", speed);
  move('B', "clockwise", speed);
  move('C', "clockwise", speed);
  move('D', "clockwise", speed);
}
void ctrlforward(int speed, int wait, int acceptance, int cycle) {

  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * (acceptance - degerr(compass)) / acceptance);

    correction = max(correction, -speed);
    Serial.println(correction);

    if (compass < 1800) {
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
void backward(int speed) {
  move('A', "anti-clockwise", speed);
  move('B', "anti-clockwise", speed);
  move('C', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
}


void ctrlbackward(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * (acceptance - degerr(compass)) / acceptance);
    correction = max(correction, -speed);

    if (compass > 1800) {
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
void right(int speed) {
  move('A', "anti-clockwise", speed);
  move('B', "clockwise", speed);
  move('C', "anti-clockwise", speed);
  move('D', "clockwise", speed);
}
void ctrlright(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 1800) {
      correction = clip255(speed * (acceptance - (3600 - compass)) / acceptance);
    } else {
      correction = clip255(speed * (acceptance - compass) / acceptance);
    }

    correction = max(correction, -speed);


    if (compass < 1800) {
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
void left(int speed) {
  move('B', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
  move('A', "clockwise", speed);
  move('C', "clockwise", speed);
}

void ctrlleft(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 1800) {
      correction = clip255(speed * (acceptance - (3600 - compass)) / acceptance);
    } else {
      correction = clip255(speed * (acceptance - compass) / acceptance);
    }



    correction = max(correction, -speed);
    if (compass < 1800) {
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


void selfRotationClockwise(int speed) {
  move('C', "anti-clockwise", speed);
  move('D', "anti-clockwise", speed);
  move('A', "clockwise", speed);
  move('B', "clockwise", speed);
}

void selfRotationAntiClockwise(int speed) {
  move('A', "anti-clockwise", speed);
  move('B', "anti-clockwise", speed);
  move('C', "clockwise", speed);
  move('D', "clockwise", speed);
}

int getCompass() {
  char valueints[8];
  int degree = 0;
  int counter = 0;
  int value = 0;

  Serial1.write(0x31);
  if (Serial1.available()) {
    Serial1.read();
  }
  while (true) {
    if (Serial1.available()) {
      valueints[counter] = Serial1.read();
      counter = (counter + 1) % 8;
      if (!counter) {
        degree = (valueints[2] - 48) * 1000 + (valueints[3] - 48) * 100 + (valueints[4] - 48) * 10 + (valueints[6] - 48);
        return degree;
      }
    }
  }

}



void printMacAddress()
{
  // get your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  devID = mac2String(mac);


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

void senddata(String id, char wasd) {
  digitalWrite(ENABLE, LOW);
  id.concat('#');
  id.concat(wasd);
  id.concat('#');
  id.concat('#');
  id.concat('#');
  id.concat('#');
  char plain[id.length() + 1];
  id.toCharArray(plain, id.length() + 1);
  if (client.connect(host, 9999)) {
    client.print(plain);
    //    client.print('#');
    //    client.print(Direction);
    //    client.print('#');
    //    client.print('#');
    //    client.print('#');
    //    client.print('#');
    client.stop();
  }
  digitalWrite(ENABLE, HIGH);

}


String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; i++)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
  }
  return s;
}

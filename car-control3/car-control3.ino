
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
  Serial2.begin(9600);


  // Print WiFi MAC address
  printMacAddress();
  //  Serial.println("Performing network scan");
  //  listNetworks();
  WiFi.begin("HIDDENWIFI", "HonestyIsTheBestPolicy");



  Serial.print("UDP broadcast on 9876");
  Udp.begin(9876);
  // Wait for UDP to know IP of remote host
  while (true) {
    Serial.print(".");
    delay(400);
    if (Udp.parsePacket()) {

      Serial.println();
      Serial.print("IP found: ");
      host = Udp.remoteIP();
      // Got IP
      Serial.println(host);

      // Break out of loop
      break;
    }
  }






  // Initialize digital pins
  pinMode(ENABLE, OUTPUT);
  pinMode(SAIN2, OUTPUT);
  pinMode(SBIN2, OUTPUT);
  pinMode(SCIN2, OUTPUT);
  pinMode(SDIN2, OUTPUT);
  // Init analog pins and enable pin
  start();
}

void loop() {

  adjustCarToNorth();

  while (true) {
    //Just a loop currently
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

// constrain input from -255 to 255
int clip255(int input) {
  return constrain(input, -255, 255);
}

//translate compass angles
//e.g. compasstranslate(900,900) = 1800;
//     compasstranslate(2700,1000) = 100;
int compasstranslate(int compass, int translate) {
  return (compass + translate) % 3600;

}

//returns error from 0
//degerr(2800) = 600;
//degerr(800) = 900;
int degerr(int input) {
  if (input < 1800) {
    return input;
  } else {
    return 3600 - input;
  }
}

//adjustCarToNorth(900) rotates the car to 90 degrees
//parameter optional
void adjustCarToNorth(int angle) {

  int trial = 0;
  //anti-jamming counter
  int compassDirection = 1;
  //1 is placeholder
  int compassDiff;
  bool clockwise;
  int targetspeed;
  while (compassDirection) {

    compassDirection = compasstranslate(getCompass(), angle);
    compassDiff = degerr(compassDirection);
    //get crucial values


    clockwise = compassDirection > 1800;
    //get direction
    targetspeed = 130;
    //placeholder speed
    if (compassDiff < 500) {
      targetspeed = map(compassDiff, 0, 500, 30, 130);
      //speed slowdown after degerr < 500
    }
    trial++;
    //add anti-jamming counter

    if (trial > 150) {
      //must've jammed up
      selfRotation(160);
      //kick it
      compassDirection = 1;
      //add placeholder to prevent instant loop escape
      delay(75);
      //arbitrary delay
      trial = 0;
      //reset anti-jam
    }
    targetspeed = min(targetspeed, trial * 5);
    //prevent sudden rough start by limiting the start speed
    if (clockwise) {
      selfRotation(-targetspeed);
    } else {
      selfRotation(targetspeed);
    }
    //perform rotation
  }
}



void newmove(char motor, int speed) {
  //negative number handler
  if (speed < 0) {
    move2(motor, false, -speed);
  } else {
    move2(motor, true, speed);
  }
}


void move2(char motor, boolean clockwise, byte speed) {
  boolean modeAD;
  boolean modeBC;
  byte pwmAD;
  byte pwmBC;
  if (clockwise) {
    modeAD = HIGH;
    modeBC = LOW;
    pwmAD = 255 - speed;
    pwmBC = speed;
  } else {
    modeAD = LOW;
    modeBC = HIGH;
    pwmAD = speed;
    pwmBC = 255 - speed;
  }

  //decide the appropriate values for motor spin

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
      //apply values
  }
}



void stop() {
  digitalWrite(ENABLE, LOW);
  //enable pin emables when it is high 
}

void start() {
  // Initialize analog pins
  analogWrite(SAIN1, 0);
  analogWrite(SBIN1, 0);
  analogWrite(SCIN1, 0);
  analogWrite(SDIN1, 0);
  //write high enable, enable it
  digitalWrite(ENABLE, HIGH);
}


/*

For all the ctrl series of the movement set
gets the degerr
as degerr gets large, the movement slowly turn into selfrotation
the as it fixes itself it gets into siderotation
then finally normal directional movement
I suggest you to lift up the car to see the error compensation in action
Details cannot be explained easily as serious code golfing was in place, 
for the sake of speedy response. 



*/



void ctrlforward(int speed, int wait, int acceptance, int cycle) {

  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * max(-acceptance, (acceptance - degerr(compass))) / acceptance);
    if (compass < 1800) {
      newmove('A', speed);
      newmove('B', speed);
      newmove('C', correction);
      newmove('D', correction);
    } else {
      newmove('A', correction);
      newmove('B', correction);
      newmove('C', speed);
      newmove('D', speed);
    }
    delay(wait);

  }

}



void ctrlbackward(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    correction = clip255(speed * max(-acceptance, (acceptance - degerr(compass))) / acceptance);
    if (compass > 1800) {
      newmove('A', -speed);
      newmove('B', -speed);
      newmove('C', -correction);
      newmove('D', -correction);
    } else {
      newmove('A', -correction);
      newmove('B', -correction);
      newmove('C', -speed);
      newmove('D', -speed);
    }
    delay(wait);

  }

}

void ctrlright(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 1800) {
      correction = clip255(speed * max(-acceptance, (acceptance - 3600 + compass)) / acceptance);
      newmove('A', -speed);
      newmove('B', correction);
      newmove('C', -correction);
      newmove('D', speed);
    } else {
      correction = clip255(speed * max(-acceptance, (acceptance - compass)) / acceptance);
      newmove('A', -correction);
      newmove('B', speed);
      newmove('C', -speed);
      newmove('D', correction);
    }
    delay(wait);

  }

}

void ctrlleft(int speed, int wait, int acceptance, int cycle) {
  int compass;
  int correction;
  for (int i = 0; i < cycle; i++) {
    compass = getCompass();
    if (compass > 1800) {
      correction = clip255(speed * max(-acceptance, (acceptance - 3600 + compass)) / acceptance);
    } else {
      correction = clip255(speed * max(-acceptance, (acceptance - compass)) / acceptance);
    }



    correction = max(correction, -speed);
    if (compass < 1800) {
      newmove('A', speed);
      newmove('B', -correction);
      newmove('C', correction);
      newmove('D', -speed);
    } else {
      newmove('A', correction);
      newmove('B', -speed);
      newmove('C', speed);
      newmove('D', -correction);
    }
    delay(wait);

  }

}

//performs rotation, negative values allowed
void selfRotation(int speed) {
  newmove('C', -speed);
  newmove('D', -speed);
  newmove('A', speed);
  newmove('B', speed);
}
//interfaces with the serial2, stores into buffer. 
int getCompass() {
  char valueints[8];
  int counter = 0;
  int value = 0;

  Serial2.write(0x31);
  if (Serial2.available()) {
    Serial2.read();
  }
  while (true) {
    if (Serial2.available()) {
      valueints[counter] = Serial2.read();
      counter = (counter + 1) % 8;
      if (!counter) {
        return (valueints[2] - 48) * 1000 + (valueints[3] - 48) * 100 + (valueints[4] - 48) * 10 + (valueints[6] - 48);
      }
    }
  }

}


//premade functions by wifiesp demo BEGINS
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

//ENDS


//Sends data through TCP, no ultrasound sent yet

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
    client.stop();
  }
  digitalWrite(ENABLE, HIGH);

}

//Mac to ID
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

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>
IPAddress host;
char devID[3] = "";

/*
  WiFiEsp example: ScanNetworks

  This example  prints the Wifi shield's MAC address, and
  scans for available Wifi networks using the Wifi shield.
  Every ten seconds, it scans again. It doesn't actually
  connect to any network, so no encryption scheme is specified.

  For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/


// Emulate Serial1 on pins 6/7 if not present

WiFiEspUDP Udp;
void setup() {


  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial3.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial3);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

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
}

void loop()
{
  // scan for existing networks

  while (true);
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

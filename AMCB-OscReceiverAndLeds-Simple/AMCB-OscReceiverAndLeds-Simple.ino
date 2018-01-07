

/*
  WiFi UDP Send and Receive String

  This sketch wait an UDP packet on localPort using a WiFi shield.
  When a packet is received an Acknowledge packet is sent to the client on port remotePort

  Circuit:
   WiFi shield attached

  created 30 December 2012
  by dlf (Metodo2 srl)

*/


#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <SimpleOSC.h>
#include <OSCCommon/OSCDecoder.h>
#include <Adafruit_NeoPixel.h>

// NETWORK ----------------------------------------------------------
char ssid[] = "AMCB-MM";     //  your network SSID (name)
char pass[] = "12321aZa";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
int keyIndex = 0;            // your network key Index number (needed only for WEP)

/*
   ---------------------------------------------------
   FIX THE IP BEFORE PUSHING SKETCH ------------------
   Or let DHCP Magic occurs !!

   Comment #define but dont forget to change IP each
   ---------------------------------------------------
*/
#define DHCP
#ifdef DHCP
IPAddress localIp;
#else
IPAddress localIp(192, 168, 0, 101);
#endif

// UDP -------------------------------------------------------------
WiFiUDP Udp;
unsigned int localPort = 8000;      // local port to listen on
uint8_t packetBuffer[255]; //buffer to hold incoming packet

// LEDS ------------------------------------------------------------
// Constructor
// Adafruit_NeoPixel(nbLeds,pin, neoPixelType t=NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripA = Adafruit_NeoPixel(35, 12);
Adafruit_NeoPixel stripB = Adafruit_NeoPixel(35, 11);

float redVal, greenVal, blueVal = 0;
float brightVal = 0;

// CODE Section ---------------------------------------------------------
void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  Serial.println("Start...");
  /*
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  */

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  controlBlink(2, 100);

  // Setup Leds -------------------------------------------------
  Serial.println("Setuping Leds...");
  stripA.begin();
  stripB.begin();
  lightForSetup(&stripA);
  lightForSetup(&stripB);

  controlBlink(2, 100);

  redVal = 1.0f;
  greenVal = 0.89f;
  blueVal = 0.77f;
  brightVal = 0.2f;

  // SO, Color Leds
  colorTheLeds(&stripA);
  colorTheLeds(&stripB);

  // Setup WiFi ----------------------------------------------------
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8, 7, 4, 2);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  controlBlink(2, 100);

  // attempt to connect to WiFi network:
  AttemptToConnect(ssid, pass);

#ifndef DHCP
  // Fix the IP Address
  WiFi.config(localIp);
#endif

  Serial.println("Connected to wifi");
  printWiFiStatus();
  // if you get a connection, report back via serial:
  Serial.println("\nStarting connection to server...");
  controlBlink(2, 100);

  // Setup UDP -------------------------------------------------
  Serial.println("Setuping UDP...");
  Udp.begin(localPort);
  controlBlink(2, 100);

  // ----
  Serial.println("Setup done...");
}

void loop() {

  // PRINT IP Address ---
  Serial.println();
  printWiFiInfos();

  if (WiFi.status() == WL_CONNECTED) {

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {

      // read the packet into packetBufffer
      int len = Udp.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len] = 0;
      OSCMessage m;
      OSCDecoder decoder;

      // Blink as Message Received ----------------------------------------
      analogWrite(LED_BUILTIN, packetBuffer[len - 1]);

      // Decode Message ----------------------------------------
      // SO Read the osc to get the color
      decoder.decode(&m, packetBuffer);
      String address = m.getOSCAddress();

      Serial.print("Address : ");
      Serial.print(address);

      if (address.startsWith("/animation")) {
        decodeAnimation(m);
      } else {
        // UNKNOWN --------------------
      }

      printLog();

    }
  } else {
    // Unconnected ---------------------------------------
    // Default Value : Brightness 50 / Color (Cold White)
    Serial.print("UnConnected : Default Values");


    redVal = 1.0f;
    greenVal = 0.89f;
    blueVal = 0.77f;
    brightVal = 0.2f;

    // SO, Color Leds
    colorTheLeds(&stripA);
    colorTheLeds(&stripB);

  }

  // SO, Color Leds
  colorTheLeds(&stripA);
  colorTheLeds(&stripB);

}

// -----------------------------------------------------------------------------
// ANIMATION SECTION -----------------------------------------------------------
void decodeAnimation(OSCMessage m) {

  String address = m.getOSCAddress();

  if (address.startsWith("/animation/color/red")) {
    redVal = m.getArgFloat(0);

    Serial.print("\tSetting RED Value : ");
    printLog();

  } else if (address.startsWith("/animation/color/blue")) {
    blueVal = m.getArgFloat(0);

    Serial.print("\tSetting BLUE Value : ");
    printLog();

  } else if (address.startsWith("/animation/color/green")) {
    greenVal = m.getArgFloat(0);

    Serial.print("\tSetting GREEN Value : ");
    printLog();

  } else if (address.startsWith("/animation/brightness")) {
    brightVal = m.getArgFloat(0);

    Serial.print("\tSetting BRIGHT Value : ");
    printLog();

  }
}

void AttemptToConnect(String _ssid, String _pass) {

  Serial.print("Before connection loop : ");
  Serial.print(" Status = ");
  Serial.print(WiFi.status());

  do {

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(_ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(_ssid, _pass);

    // wait 10 seconds for connection:
    for (int idxWait = 1; idxWait <= 10; idxWait++) {

      // Blink one second---
      controlBlink(2, 1000);

      // Print
      Serial.print("Waiting for connection.... ");
      Serial.print(idxWait);
      Serial.print(" / 10 seconds ");
      Serial.println();

    }

  } while ( status != WL_CONNECTED);

}

void printWiFiInfos() {

  localIp = WiFi.localIP();
  Serial.print("IP:");
  Serial.print(localIp);
  Serial.print(" UDP RemoteIp:");
  IPAddress remoteIp = Udp.remoteIP();
  Serial.print(remoteIp);
  Serial.print(" UDP Remote port:");
  Serial.print(Udp.remotePort());

}

void printLog() {
  Serial.print(" [R,G,B,Brightness] : ");
  Serial.print("[");
  Serial.print(redVal);
  Serial.print(",");
  Serial.print(greenVal);
  Serial.print(",");
  Serial.print(blueVal);
  Serial.print(",");
  Serial.print(brightVal);
  Serial.print("]");
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  localIp = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(localIp);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

}

// LEDS functions ------------------------------------------
uint8_t red(uint32_t c) {
  return (c >> 8);
}
uint8_t green(uint32_t c) {
  return (c >> 16);
}
uint8_t blue(uint32_t c) {
  return (c);
}

// Setup sequence
void lightForSetup(Adafruit_NeoPixel * myStrip) {

  // Half the leds --
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(255 * (idxLed % 2), 255 * (idxLed % 2), 255 * (idxLed % 2)));
  }
  myStrip->setBrightness(255);
  myStrip->show();

  // Wait
  delay(1000);

  // Half the other leds --
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2)));
  }
  myStrip->setBrightness(255);
  myStrip->show();

  // Wait
  delay(1000);

  // All Off --
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(0, 0, 0));
  }
  myStrip->setBrightness(0);
  myStrip->show();

}

void colorTheLeds(Adafruit_NeoPixel * myStrip) {
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(255 * redVal, 255 * greenVal, 255 * blueVal));
  }
  myStrip->setBrightness(255 * brightVal);
  myStrip->show();
}

// Control Section -----------------------------------------
// Takes a 100 ms --
void controlBlink(int nbBlink, int delayInMs) {
  for (int n = 0; n < nbBlink; n++) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(delayInMs / 2);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(delayInMs / 2);                       // wait for a second
  }
}



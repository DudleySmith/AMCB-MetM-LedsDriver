

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
char ssid[] = "Manufactures";     //  your network SSID (name)
char pass[] = "1pourtoustouspour1";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
int keyIndex = 0;            // your network key Index number (needed only for WEP)
IPAddress localIp;

// UDP -------------------------------------------------------------
WiFiUDP Udp;
unsigned int localPort = 8000;      // local port to listen on
uint8_t packetBuffer[255]; //buffer to hold incoming packet

// LEDS ------------------------------------------------------------
#define startBrightness  50
// Constructor
// Adafruit_NeoPixel(nbLeds,pin, neoPixelType t=NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripA = Adafruit_NeoPixel(60, 12);
Adafruit_NeoPixel stripB = Adafruit_NeoPixel(60, 11);

int redVal, greenVal, blueVal = 0;
int brightVal = 0;

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
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  controlBlink(2, 100);

  Serial.println("Connected to wifi");
  printWiFiStatus();
  // if you get a connection, report back via serial:
  Serial.println("\nStarting connection to server...");
  controlBlink(2, 100);

  // Setup UDP -------------------------------------------------
  Serial.println("Setuping UDP...");
  Udp.begin(localPort);
  controlBlink(2, 100);

}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // PRINT IP Address ---
    Serial.println();
    printWiFiInfos();

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    OSCMessage m;
    OSCDecoder decoder;

    // Blink Message ----------------------------------------
    Serial.print(" ");
    Serial.print("PACKET BUFFER ---------------------------");
    Serial.print((int)(packetBuffer[len - 1]));
    Serial.print(" ");
    analogWrite(LED_BUILTIN, packetBuffer[len - 1]);

    // Decode Message ----------------------------------------
    // SO Read the osc to get the color
    decoder.decode(&m, packetBuffer);
    String address = m.getOSCAddress();

    if (address.startsWith("/color/red")) {
      redVal = m.getArgInt32(0);

      Serial.print("\tSetting RED Value : ");
      printLog();

    } else if (address.startsWith("/color/blue")) {
      blueVal = m.getArgInt32(0);

      Serial.print("\tSetting BLUE Value : ");
      printLog();

    } else if (address.startsWith("/color/green")) {
      greenVal = m.getArgInt32(0);

      Serial.print("\tSetting GREEN Value : ");
      printLog();

    } else if (address.startsWith("/color/brightness")) {
      brightVal = m.getArgInt32(0);

      Serial.print("\tSetting BRIGHT Value : ");
      printLog();

    } else {

      Serial.print("\tAddress Unknown !!!!! : ");
      Serial.print(address);
      printLog();

    }

  }

  // SO, Color Leds
  colorTheLeds(&stripA);
  colorTheLeds(&stripB);

}

void printWiFiInfos() {
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
void lightForSetup(Adafruit_NeoPixel *myStrip) {

  // Half the leds --
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(255 * (idxLed % 2), 255 * (idxLed % 2), 255 * (idxLed % 2)));
  }
  myStrip->setBrightness(startBrightness);
  myStrip->show();

  // Wait
  delay(1000);

  // Half the other leds --
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2)));
  }
  myStrip->setBrightness(startBrightness);
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

void colorTheLeds(Adafruit_NeoPixel *myStrip) {
  for (int idxLed = 0; idxLed < myStrip->numPixels(); idxLed++) {
    myStrip->setPixelColor(idxLed, myStrip->Color(redVal, greenVal, blueVal));
  }
  myStrip->setBrightness(brightVal);
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



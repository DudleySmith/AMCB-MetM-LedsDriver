

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
#define stripAPin       12
#define NbLeds          60
#define startBrightness  50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NbLeds, stripAPin, NEO_GRB + NEO_KHZ800);
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
  strip.begin();
  lightForSetup();

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

  // Blink quickly each loop
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    /*
      Serial.print("Received packet of size ");
      Serial.print(packetSize);
      Serial.print("From ");
      IPAddress remoteIp = Udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
    */

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    OSCMessage m;
    OSCDecoder decoder;

    // Blink Message ----------------------------------------
    if (packetBuffer[len / 2] > 127) {
      digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
    } else {
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    }

    /*
      Serial.print("Rough Content : ");
      for (int i = 0; i < len; i++) {
      Serial.print(char(packetBuffer[i]));
      }
      Serial.println();
    */

    // Decode Message ----------------------------------------
    decoder.decode(&m, packetBuffer);
    int argsNum = m.getArgsNum();

    /*
      Serial.print("Readable Content : ");
      Serial.print("Nb Args :");
      Serial.print(argsNum);
    */

    for (int j = 0; j < argsNum; j++) {
      // Float args
      char type = m.getArgTypeTag(j);
      // Print ----------------
      /*
        Serial.print("| Address : ");
        Serial.print(m.getOSCAddress());
        Serial.print("| Arg [");
        Serial.print(j);
        Serial.print("] ");
        Serial.print("| Type:");
        Serial.print(type);
        Serial.print("| Value:");
      */

      switch (type) {
        case kTagInt32:
          // Int Argument
          Serial.print(m.getArgInt32(j));
          break;
        case kTagFloat:
          // Int Argument
          Serial.print(m.getArgFloat(j));
          break;
        case kTagString:
          // Int Argument
          char *argString;
          m.getArgString(j, argString);
          Serial.print(argString);
          break;

        case kTagBlob:
        case kTagMidi:
        case kTagRgba:
        case kTagTrue:
        case kTagFalse:
        default:
          // NOT IMPLEMENTED, and default
          Serial.print("Type unknown");
          break;
      }

    }
    Serial.println();

    // SO Read the osc to get the color
    String address = m.getOSCAddress();

    if (address.startsWith("/color/red")) {
      redVal = m.getArgInt32(0);

      Serial.print("Setting RED Value : ");
      printLog();

    } else if (address.startsWith("/color/blue")) {
      blueVal = m.getArgInt32(0);

      Serial.print("Setting BLUE Value : ");
      printLog();

    } else if (address.startsWith("/color/green")) {
      greenVal = m.getArgInt32(0);

      Serial.print("Setting GREEN Value : ");
      printLog();

    } else if (address.startsWith("/color/brightness")) {
      brightVal = m.getArgInt32(0);

      Serial.print("Setting BRIGHT Value : ");
      printLog();

    } else {

      Serial.print("Address Unknown !!!!! : ");
      Serial.print(address);
      Serial.print(" length : ");
      Serial.print(address.length());
      printLog();

    }


    /*
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
      Serial.println("Reply:");
      Serial.println(ReplyBuffer);
    */
  }

  // SO, Color Leds
  for (int idxLed = 0; idxLed < strip.numPixels(); idxLed++) {
    strip.setPixelColor(idxLed, strip.Color(redVal, greenVal, blueVal));
  }
  strip.setBrightness(brightVal);
  strip.show();


}

void printLog() {

  Serial.print("UDP RemoteIp ");
  IPAddress remoteIp = Udp.remoteIP();
  Serial.print(remoteIp);
  Serial.print(", port ");
  Serial.print(Udp.remotePort());

  Serial.print("IP Address : ");
  Serial.print(localIp);
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
  Serial.println();

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
void lightForSetup() {

  // Half the leds --
  for (int idxLed = 0; idxLed < strip.numPixels(); idxLed++) {
    strip.setPixelColor(idxLed, strip.Color(255 * (idxLed % 2), 255 * (idxLed % 2), 255 * (idxLed % 2)));
  }
  strip.setBrightness(startBrightness);
  strip.show();

  // Wait
  delay(1000);

  // Half the other leds --
  for (int idxLed = 0; idxLed < strip.numPixels(); idxLed++) {
    strip.setPixelColor(idxLed, strip.Color(255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2), 255 * ((idxLed + 1) % 2)));
  }
  strip.setBrightness(startBrightness);
  strip.show();

  // Wait
  delay(1000);

  // All Off --
  for (int idxLed = 0; idxLed < strip.numPixels(); idxLed++) {
    strip.setPixelColor(idxLed, strip.Color(0, 0, 0));
  }
  strip.setBrightness(0);
  strip.show();

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



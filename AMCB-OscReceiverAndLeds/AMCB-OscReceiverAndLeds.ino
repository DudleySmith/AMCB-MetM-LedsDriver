

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

// UDP -------------------------------------------------------------
WiFiUDP Udp;
unsigned int localPort = 8000;      // local port to listen on
uint8_t packetBuffer[255]; //buffer to hold incoming packet

// LEDS ------------------------------------------------------------
#define stripAPin       12
#define NbLeds          10
#define FullBrightness  50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NbLeds, stripAPin, NEO_GRB + NEO_KHZ800);

int gammaArray[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

int redVal, greenVal, blueVal = 255;
int brightVal = 0;

// CODE Section ---------------------------------------------------------
void setup() {

  Serial.write(27);
  Serial.print("[2J"); // clear screen
  Serial.write(27); // ESC
  Serial.print("[H"); // cursor to home

  // Setup WiFi ----------------------------------------------------
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8, 7, 4, 2);

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
  // if you get a connection, report back via serial:
  Serial.println("\nStarting connection to server...");

  // Setup UDP -------------------------------------------------
  Serial.println("Setuping UDP...");
  Udp.begin(localPort);

  // Setup Leds -------------------------------------------------
  Serial.println("Setuping Leds...");
  strip.setBrightness(FullBrightness);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    OSCMessage m;
    OSCDecoder decoder;

    // PRINT Message ----------------------------------------
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

    Serial.print("Readable Content : ");
    Serial.print("Nb Args :");
    Serial.print(argsNum);

    for (int j = 0; j < argsNum; j++) {
      // Float args
      char type = m.getArgTypeTag(j);
      // Print ----------------
      Serial.print("| Address : ");
      Serial.print(m.getOSCAddress());
      Serial.print("| Arg [");
      Serial.print(j);
      Serial.print("] ");
      Serial.print("| Type:");
      Serial.print(type);
      Serial.print("| Value:");

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
      Serial.print(redVal);
      Serial.println();

    } else if (address.startsWith("/color/blue")) {
      blueVal = m.getArgInt32(0);

      Serial.print("Setting BLUE Value : ");
      Serial.print(blueVal);
      Serial.println();

    } else if (address.startsWith("/color/green")) {
      greenVal = m.getArgInt32(0);

      Serial.print("Setting GREEN Value : ");
      Serial.print(greenVal)  ;
      Serial.println();

    } else if (address.startsWith("/color/brightness")) {
      brightVal = m.getArgInt32(0);

      Serial.print("Setting BRIGHT Value : ");
      Serial.print(brightVal);
      Serial.println();

    } else {
      
      Serial.print("Address Unknown !!!!! : ");
      Serial.print(address);
      Serial.print(" length : ");
      Serial.print(address.length());
      Serial.println();

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

  Serial.println("Leds driving : ");
  Serial.print("[R,G,B,Brightness] : ");
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

  strip.show();


}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

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





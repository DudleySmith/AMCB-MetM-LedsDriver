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

char ssid[] = "Manufactures";     //  your network SSID (name)
char pass[] = "1pourtoustouspour1";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 8000;      // local port to listen on

uint8_t packetBuffer[255]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8, 7, 4, 2);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
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

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
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
    Serial.print("Rough Content : ");
    for (int i = 0; i < len; i++) {
      Serial.print(char(packetBuffer[i]));
    }
    Serial.println();

    // Decode Message ----------------------------------------
    decoder.decode(&m, packetBuffer);
    Serial.print("Readable Content : ");
    Serial.print("Nb Args :");
    int argsNum = m.getArgsNum();
    for (int j = 0; j < argsNum; j++) {
      // Float args
      char type = m.getArgTypeTag(j);
      // Print ----------------
      Serial.print(" Arg [");
      Serial.print(j);
      Serial.print("] ");
      Serial.print(" Type:");
      Serial.print(type);
      Serial.print(" Value:");
      
      switch (type) {
        case kTagInt32:
          // Int Argument
          Serial.print(m.addArgInt32(j));
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
    /*
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
      Serial.println("Reply:");
      Serial.println(ReplyBuffer);
    */
  }
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





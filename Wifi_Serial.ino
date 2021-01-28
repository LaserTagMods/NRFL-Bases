#include <WiFi.h>
#include <HardwareSerial.h>
  
// debug log, set to 1 to enable
#define ENABLE_DEBUG_LOG 0

// Stupid need to assign a GunID
int GunID = 9;
  
// wifi config
const char* ssid     = "Section9";
const char* password = "R1ch@rds0nM@n0r";

//Server advertizement response variables
String ServerString = "$BRXSERVER";
String BRX = "$BRX,";
String IPResponse = "";

// ethernet config
//const IPAddress local_IP(10, 10, 0, 37);
//const IPAddress gateway(10, 10, 0, 1);
//const IPAddress subnet(255, 255, 255, 0);
//const IPAddress primaryDNS(8, 8, 8, 8);
//const IPAddress secondaryDNS(8, 8, 4, 4);

// ********************************************************  
// UDP Listener Stuff
// ********************************************************
unsigned int localPort = 3333;      // local port to listen on

char packetBuffer[20]; //buffer to hold incoming packet
char ReplyBuffer[20]; // = "acknowledged"; // a string to send back

WiFiUDP Udp;

// *********************************************************

// rs-server config
// const int serverIP = (IPADDRESS) //Somehow set the app IP here
const int serverPort = 3333;
  
// rs port config
const int baudrate = 115200;
const int rs_config = SERIAL_8N1;

#define SERIAL1_RXPIN 16 // TO BRX TX and BLUETOOTH RX
#define SERIAL1_TXPIN 17 // TO BRX RX and BLUETOOTH TX
  
// reading buffer config
#define BUFFER_SIZE 20
  
// global objects
WiFiServer server;
byte buff[BUFFER_SIZE];
  
void debug_log(char* str) {
#if ENABLE_DEBUG_LOG == 1
   Serial.println(str);
#endif
}
  
void setup() {
// init rs port
   Serial.begin(baudrate, rs_config);
   
// setting up the serial pins for sending data to BRX
   Serial1.begin(baudrate, rs_config, SERIAL1_RXPIN, SERIAL1_TXPIN);   
   
// init wifi connection
//if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//       debug_log("Failed to configure network settings");
//}
   WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
       debug_log("connecting to WiFi network");
       delay(500);
}
  
#if ENABLE_DEBUG_LOG == 1
   Serial.println("connected to WiFi");
   Serial.println("IP adddr: ");
   Serial.println(WiFi.localIP());
#endif
   delay(1000);
  
//start server
   server = WiFiServer(serverPort);
   server.begin();
   delay(1000);
   debug_log("server started");

//Start Udp
   Udp.begin(localPort);

//Trying to get the printWifiStatus thing to work
   Serial.println("Connected to wifi");
   printWifiStatus();

}

void printWifiStatus() {

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
  
void loop() {
// ********************************************************************
// Sending to the BRX and Receiving from the BRX
// ********************************************************************

// if there's data available, read a packet
  IPAddress ip = WiFi.localIP(); //get IP assigned to ESP32
  int packetSize = Udp.parsePacket();
  IPAddress remoteIp = Udp.remoteIP();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

// read the packet into packetBuffer
    int len = Udp.read(packetBuffer, 20);
    if (len > 0) {
      packetBuffer[len] = 0;
    Serial1.println(packetBuffer); //Sends the recieved string to the BRX
    Serial.print("From APP: ");
    Serial.println(packetBuffer);
    }
    String ServerCheck(packetBuffer);    
    if (ServerCheck == ServerString) {  //Checks for $BRXSERVER in the UDP packetBuffer
      Udp.beginPacket(remoteIp, serverPort);
      Udp.print(BRX);
      Udp.print(GunID);
      Udp.print(",");
      Udp.println(ip);
      Udp.endPacket();
      Serial.print("From BRX: ");
      Serial.print(BRX);
      Serial.print(GunID);
      Serial.print(",");
      Serial.println(ip);
      }
    }
  
// ***This is slightly delayed from the gun, like it is waiting for a break in input
// read data from serial and send to wifi client
  int serial1Received = Serial1.available();
  if (serial1Received) {
    int len = Serial1.readBytes(ReplyBuffer, 20);
    if (len > 0) {
      ReplyBuffer[len] = 0;
    }
    Serial.print("From Gun: ");
    Serial.println(ip);
    Serial.println(ReplyBuffer);
    
    Udp.beginPacket(remoteIp, serverPort);
    Udp.print(ip);
    Udp.println(",");
    Udp.println(ReplyBuffer);
    Udp.endPacket();
  }
}

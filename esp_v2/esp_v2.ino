/*
TODO:
See if it flies :p
Master handler
Gently stop quad method
Add authorization
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <SoftwareSerial.h>

//#define RX_PIN_G 7
//#define TX_PIN_G 6
//#define RX_PIN_A 5
//#define TX_PIN_A 4
#define BAUD_RATE 9600

// Primary commands
#define ARM 10
#define THR 11
#define AIL 12
#define ELE 13
#define RUD 14
#define AUX 15
#define STP 20

// ESC limits
#define ESC_MAX_VALUE 2190  // 168
#define ESC_MIN_VALUE 800   // -171
 
#define ESC_THR_MIN 1150    // 0 Idle
#define ESC_THR_MAX 2390    // 157
//End ESC limits

// SW serial setup
//SoftwareSerial ss_arduino( RX_PIN_A, TX_PIN_A );
//SoftwareSerial ss_gps( RX_PIN_G, TX_PIN_G );

// WiFi setup
const char* ssid = "D-Link_DIR-816";
const char* password = "Adwait@54321";

// Web server setup
ESP8266WebServer server(80);

// If found contact Akshay +91 70 222 84 255. This drone is private property. And currently being used for research projects. Kindly return.

void ArmHandler(){
  if(Serial.availableForWrite()){
    char a[] = {'1','0','0','0','0','0','a'};
    Serial.write(a, 7);
  }
  server.send(200, "text/plain", "Arming Quad");
}

void ShowStopperHandler(){
  if(Serial.availableForWrite()){
    char a[] = {'2','0','0','0','0','0','a'};
    Serial.write(a, 7);
  }
  server.send(200, "text/plain", "Force Stopping Quad");
}

void MasterHandler(){
  int primary_cmd;
  primary_cmd = server.arg(0).toInt();
  switch(primary_cmd){
    case ARM: ArmHandler(); break;
    case STP: ShowStopperHandler(); break;
    default: 
      String pri = server.arg(0);
      String val = server.arg(2);
      char a[7] = {pri[0],pri[1],val[0],val[1],val[2],val[3]};
      if(Serial.availableForWrite()){
        Serial.write(a,7);
      }
  }
  server.send(200, "text/plain", "OK");  
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void InitServerHandlers(){
  server.on("/drone", MasterHandler);
  server.onNotFound(handleNotFound);
  server.begin();
}

void WiFiEvent(WiFiEvent_t event){
  switch(event){
    case WIFI_EVENT_STAMODE_GOT_IP:
      InitServerHandlers();
      break;
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, password);
}

void loop() {
  server.handleClient();
}

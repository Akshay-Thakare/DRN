/*
TODO:
See if it flies :p
Gently stop quad method
Add authorization
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define RX_PIN_G 7
#define TX_PIN_G 6
#define RX_PIN_A 5
#define TX_PIN_A 4
#define BAUD_RATE 9600

// Primary commands
#define ARM 10
#define THR_MIN 1116
#define THR_SCU 1117
#define THR_SCD 1118
#define THR_MAX 1119

#define AIL_MIN 1216
#define AIL_SCU 1217
#define AIL_SCD 1218
#define AIL_MAX 1219

#define ELE_MIN 1316
#define ELE_SCU 1317
#define ELE_SCD 1318
#define ELE_MAX 1319

#define RUD_MIN 1416
#define RUD_SCU 1417
#define RUD_SCD 1418
#define RUD_MAX 1419

#define AUX_MIN 1516
#define AUX_SCU 1517
#define AUX_SCD 1518
#define AUX_MAX 1519

#define STP 20

// Mod commands for future use. When mod values(size:4) would be send along too
#define THR 11
#define AIL 12
#define ELE 13
#define RUD 14
#define AUX 15

#define MIN 16
#define SCU 17
#define SCD 18
#define MAX 19

// ESC limits
#define ESC_MAX_VALUE 2190  // 168
#define ESC_MIN_VALUE 800   // -171
 
#define ESC_THR_MIN 1150    // 0 Idle
#define ESC_THR_MAX 2390    // 157
//End ESC limits

#define AUXILLARY 11     
#define RUDDER 10         // yaw
#define ELEVATOR 9        // pitch
#define AILERON 6         // roll
#define THROTTLE 5

// SW serial setup
SoftwareSerial ss_arduino( RX_PIN_A, TX_PIN_A );
//SoftwareSerial ss_gps( RX_PIN_G, TX_PIN_G );

// WiFi setup
const char* ssid = "D-Link_DIR-816";
const char* password = "Adwait@54321";

// Web server setup
ESP8266WebServer server(80);

String message="If found contact Akshay +91 70 222 84 255. This drone is private property. And currently being used for research projects. Kindly return.";

void handleRoot(){
  server.send(200, "text/plain", message);
}

void ArmHandler(){
  Serial.println("arming");
  ss_arduino.print(ARM*1000000); //*1000000
  server.send(200, "text/plain", "Arming Quad");
}

void ShowStopperHandler(){
  Serial.println("stopping");
  ss_arduino.print(STP*1000000);
  server.send(200, "text/plain", "Force Stopping Quad");
}

// Scale percentage must be fixed. It should not cross upper limit
int ProcessSpeedScale(int cmd, int scalePercentage, bool forThrottle){
  int val = -1;
  if(scalePercentage>100) return val;  
  if(forThrottle){
    val = (int)(ESC_THR_MIN*(1.0+scalePercentage*0.01));
//    server.send(200, "text/plain", (String)val);
    if (val>ESC_THR_MAX) val=ESC_THR_MAX;
  } else {
    val = (int)(ESC_MIN_VALUE*(1.0+scalePercentage*0.01));
    if (val>ESC_MAX_VALUE) val=ESC_MAX_VALUE;
  }
  return cmd*10000+val;
}

void ThrottleHandler(int mod_cmd, int scale_percent){
  switch(mod_cmd){
    case MIN: ss_arduino.write(THR_MIN*10000+ESC_THR_MIN); server.send(200, "text/plain", "MIN Throttle"); break;
    case SCD: ss_arduino.write( ProcessSpeedScale(THR_SCD, scale_percent, true) ); server.send(200, "text/plain", "Scale Down Throttle"); break;
    case SCU: ss_arduino.write( ProcessSpeedScale(THR_SCU, scale_percent, true) ); server.send(200, "text/plain", "Scale Up Throttle"); break;
    case MAX: ss_arduino.write(THR_MAX*10000+ESC_THR_MAX); server.send(200, "text/plain", "Max Throttle"); break;
    default: server.send(404, "text/plain", "Invalid Throttle control argument"); break;
  }  
}

void GeneralProcessor(int primary_cmd, int mod_cmd, int scale_percent){
  if (scale_percent<100){
    if(primary_cmd==THR){
      switch(mod_cmd){
        case MIN: ss_arduino.print((primary_cmd*100+mod_cmd)*10000+ESC_THR_MIN); server.send(200, "text/plain", "MIN "+primary_cmd); break;
        case SCD: ss_arduino.print( ProcessSpeedScale((primary_cmd*100+mod_cmd),scale_percent, true)); server.send(200, "text/plain", "SCD "+primary_cmd); break;
        case SCU: ss_arduino.print( ProcessSpeedScale((primary_cmd*100+mod_cmd),scale_percent, true)); server.send(200, "text/plain", "SCU "+primary_cmd); break;
        case MAX: ss_arduino.print((primary_cmd*100+mod_cmd)*10000+ESC_THR_MAX); server.send(200, "text/plain", "MAX "+primary_cmd); break;
      }
    } else {
      switch(mod_cmd){
        case MIN: ss_arduino.print((primary_cmd*100+mod_cmd)*10000+ESC_MIN_VALUE); server.send(200, "text/plain", "MIN "+primary_cmd); break;
        case SCD: ss_arduino.print( ProcessSpeedScale((primary_cmd*100+mod_cmd),scale_percent, false)); server.send(200, "text/plain", "SCD "+primary_cmd); break;
        case SCU: ss_arduino.print( ProcessSpeedScale((primary_cmd*100+mod_cmd),scale_percent, false)); server.send(200, "text/plain", "SCU "+primary_cmd); break;
        case MAX: ss_arduino.print((primary_cmd*100+mod_cmd)*10000+ESC_MAX_VALUE); server.send(200, "text/plain", "MAX "+primary_cmd); break;
      }
    }
  } else {    
    int calculatedValue = (primary_cmd*100+mod_cmd)*10000+scale_percent;
    Serial.print("TOLO : ");
    Serial.println(calculatedValue);
    ss_arduino.println(calculatedValue); 
    server.send(200, "text/plain", "OK");
  }
}

void MasterHandler(){
  
  int server_arg_count = server.args();
//  Serial.println(server_arg_count);
//  Serial.println(server.arg(2).toInt());
//  Serial.println(server.arg(1).toInt());
//  Serial.println(server.arg(0).toInt());
  int primary_cmd, mod_cmd, scale_percent;
  
  switch(server_arg_count){
    case 3: scale_percent = server.arg(2).toInt();
    case 2: mod_cmd = server.arg(1).toInt();
    case 1: primary_cmd = server.arg(0).toInt(); break;
    default: Serial.print("Something went terribly wrong"); message="ERROR:Parsing arguments"; return;
  }

  // TODO :: Error handling

  switch(primary_cmd){
    case ARM: ArmHandler(); break;
    case STP: ShowStopperHandler(); break;
    default: GeneralProcessor(primary_cmd, mod_cmd, scale_percent); break;
  }
  
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

  server.on("/", handleRoot);
  server.on("/drone", MasterHandler);
//  server.on("/arm", ArmHandler);
//  server.on("/stop", ShowStopperHandler);
//  server.on("/throttle", ThrottleHandler);
//  server.on("/aileron", AileronHandler);
//  server.on("/elevator", ElevatorHandler);
//  server.on("/rudder", RudderHandler);
//  server.on("/auxillary", AuxillaryHandler);

//  server.on("/inline", [](){
//    server.send(200, "text/plain", "this works as well");
//  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void WiFiEvent(WiFiEvent_t event){
  Serial.printf("[WiFi-event] event: %d\n", event);

  // See TestEspApi example to see event names
  switch(event){
    case WIFI_EVENT_STAMODE_CONNECTED:
      Serial.println("Wifi connected");
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:      
      Serial.println("IP address : ");
      Serial.println(WiFi.localIP());
      InitServerHandlers();
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      Serial.println("WiFi lost connection");
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      Serial.println("WARN: WiFi auth mode changed");
      break;
    case WIFI_EVENT_MAX:
      Serial.println("ERROR: EVENT_MAX unknown");
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      Serial.println("ERROR: EVENT_SOFTAPMODE_STACONNECTED should never occur");
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      Serial.println("ERROR: EVENT_SOFTAPMODE_STADISCONNECTED should never occur");
      break;
    default:
      Serial.println("ERROR: Unknown");
      break;
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  ss_arduino.begin(BAUD_RATE);
//  ss_gps.begin(baudRate);  

  // delete old config
  WiFi.disconnect(true);
  
  delay(1000);

  // Initialize wifi event handlers
  WiFi.onEvent(WiFiEvent);

  // Connect to wifi
  WiFi.begin(ssid, password);
}

void loop() {

//  if(Serial.available()){
//    Serial.print(Serial.readString());
//  }

  if(ss_arduino.available()){
    message = ss_arduino.readString();
//    Serial.print("incoming : ");
//    Serial.println(message);
  }
  
  server.handleClient();
}

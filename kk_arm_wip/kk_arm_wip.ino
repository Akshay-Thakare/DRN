#include <Servo.h>
#include <NeoSWSerial.h>

// Software serial
#define RX_PIN_E 4
#define TX_PIN_E 3
//END Software serial

// KK mini pin configs
#define AUXILLARY 11     
#define RUDDER 10         // yaw
#define ELEVATOR 9        // pitch
#define AILERON 6         // roll
#define THROTTLE 5
//END KK mini pin configs

// ESC limits
#define ESC_MAX_VALUE 1910  // 168
#define ESC_MIN_VALUE 1090   // -171
 
#define ESC_THR_MIN 1150    // 0 Idle
#define ESC_THR_MAX 1940    // 157
 
#define ESC_ARM_DELAY 7000
#define ESC_ARM_RUD_SPEED 800
//End ESC limits

// Primary commands
#define ARM 10
#define THR 11
#define THR_MIN 1116
#define THR_SCU 1117
#define THR_SCD 1118
#define THR_MAX 1119

#define AIL 12
#define AIL_MIN 1216
#define AIL_SCU 1217
#define AIL_SCD 1218
#define AIL_MAX 1219

#define ELE 13
#define ELE_MIN 1316
#define ELE_SCU 1317
#define ELE_SCD 1318
#define ELE_MAX 1319

#define RUD 14
#define RUD_MIN 1416
#define RUD_SCU 1417
#define RUD_SCD 1418
#define RUD_MAX 1419

#define AUX 15
#define AUX_MIN 1516
#define AUX_SCU 1517
#define AUX_SCD 1518
#define AUX_MAX 1519

#define STP 20
//END Primary commands

// SW serial setup
NeoSWSerial ss_esp( RX_PIN_E, TX_PIN_E );

Servo aux, rud, ele, ail, thr;
int current_aux=0, current_rud=0, current_ele=0, current_ail=0, current_thr=0;
 
void setup() {
  Serial.begin(9600);
  ss_esp.begin(9600);
  Serial.println("Program begin...");
 
  aux.attach(AUXILLARY);
  rud.attach(RUDDER);
  ele.attach(ELEVATOR);
  ail.attach(AILERON);
  thr.attach(THROTTLE);
 
  aux.writeMicroseconds(0);     // MAX = ON, MIN = OFF
  rud.writeMicroseconds(0);     // MAX = LEFT, MIN = RIGHT
  ele.writeMicroseconds(0);     // MAX = BACK, MIN = FORWARD
  ail.writeMicroseconds(0);     // MAX = RIGHT, MIN = LEFT
  thr.writeMicroseconds(0);     // MAX = FULL, MIN = m
 
}
 
//https://www.youtube.com/watch?v=8NAXSBCtSLY
void armQuad(){
  Serial.println("Arming quad");
  thr.writeMicroseconds(ESC_MIN_VALUE);
  rud.write(ESC_ARM_RUD_SPEED);
  delay(ESC_ARM_DELAY);
}
 
void valueMin(char part){
  Serial.println("value min");
  switch(part){
    case 'a' : current_aux=ESC_MIN_VALUE; aux.writeMicroseconds(ESC_MIN_VALUE); break;
    case 'r' : current_rud=ESC_MIN_VALUE; rud.writeMicroseconds(ESC_MIN_VALUE); break;
    case 'e' : current_ele=ESC_MIN_VALUE; ele.writeMicroseconds(ESC_MIN_VALUE); break;
    case 'l' : current_ail=ESC_MIN_VALUE; ail.writeMicroseconds(ESC_MIN_VALUE); break;
    case 't' : current_thr=ESC_THR_MIN; thr.writeMicroseconds(ESC_THR_MIN); break;
    default: showStopper(); break;
  }
  delay(2);
}
 
void valueMax(char part){
  Serial.println("value max");
  switch(part){
    case 'a' : current_aux=ESC_MAX_VALUE; aux.writeMicroseconds(ESC_MAX_VALUE); break;
    case 'r' : current_rud=ESC_MAX_VALUE; rud.writeMicroseconds(ESC_MAX_VALUE); break;
    case 'e' : current_ele=ESC_MAX_VALUE; ele.writeMicroseconds(ESC_MAX_VALUE); break;
    case 'l' : current_ail=ESC_MAX_VALUE; ail.writeMicroseconds(ESC_MAX_VALUE); break;
    case 't' : current_thr=ESC_THR_MAX; thr.writeMicroseconds(ESC_THR_MAX); break;
    default: showStopper(); break;
  }
  delay(2);
}
 
void valueScale(char part, int delta){
  Serial.println("value scale");
  switch(part){
    case 'a' : current_aux+=delta; aux.writeMicroseconds(current_aux); break;
    case 'r' : current_rud+=delta; rud.writeMicroseconds(current_rud); break;
    case 'e' : current_ele+=delta; ele.writeMicroseconds(current_ele); break;
    case 'l' : current_ail+=delta; ail.writeMicroseconds(current_ail); break;
    case 't' : current_thr+=delta; thr.writeMicroseconds(current_thr); break;
    default: showStopper(); break;
  }
  delay(2);
}
 
void showStopper(){
  Serial.println("show stopper");
  aux.writeMicroseconds(0);
  rud.writeMicroseconds(0);
  ele.writeMicroseconds(0);
  ail.writeMicroseconds(0);
  thr.writeMicroseconds(0);
}
 
int getNumber(String base){
  // Arduino string end does not contain trailing null character
  if(base.charAt(2)=='-'){
    return (-1 * base.substring(3,base.length()).toInt());
  } else {
    return base.substring(2,base.length()).toInt();
  }
}
 
// TODO :: Print current values
 
void loop(){

//  thr.writeMicroseconds(1150);      // THR min 0
//  thr.writeMicroseconds(1940);      // THR max 100

//  ail.writeMicroseconds(1090);        // Left max
//  ail.writeMicroseconds(1500);      // mid
//  ail.writeMicroseconds(1910);        // Right max

//  ele.writeMicroseconds(1090);        // Forward max
//  ele.writeMicroseconds(1500);      // mid
//  ele.writeMicroseconds(1910);        // Back max

//  rud.writeMicroseconds(1090);        // Right max
//  rud.writeMicroseconds(1500);      // mid
//  rud.writeMicroseconds(1910);        // Left max

//  aux.writeMicroseconds(1090);        // stabilization off
//  aux.writeMicroseconds(1500);        // NA
//  aux.writeMicroseconds(1910);        // stabilization on
  
  if (Serial.available()){
    String m = Serial.readString();
    switch(m.charAt(0)){
      case '0' : armQuad(); break;
     case '1' : valueMin(m.charAt(1)); break;
      case '2' : valueMax(m.charAt(1)); break;
      case '3' : valueScale(m.charAt(1), getNumber(m)); break;
      default : showStopper(); break;   // STOP code
    }
  }

  if(ss_esp.available()){
//    Serial.print("Icoming message : ");
//    Serial.println(ss_esp.read());
    String incoming = ss_esp.readString();
    int primary_cmd = incoming.substring(0,2).toInt();
//    int mod_cmd = incoming.substring(2,4).toInt();
    int val = incoming.substring(4,10).toInt();
    Serial.print("incoming : ");
    Serial.println(primary_cmd);
//    Serial.println(mod_cmd);
//    Serial.println(val);
//    int cmd = incoming/1000000;
//    int val = incoming%10000;
//    Serial.println("ss_esp.read() : "+ss_esp.read());    
//    Serial.println("cmd : "+cmd);
//    Serial.println("val : "+val);
    switch(primary_cmd){
      case ARM: armQuad(); break;
      case STP: showStopper(); break;
      case THR: thr.writeMicroseconds(val); break;
      case AIL: ail.writeMicroseconds(val); break;
      case ELE: ele.writeMicroseconds(val); break;
      case RUD: rud.writeMicroseconds(val); break;
      case AUX: aux.writeMicroseconds(val); break;
      default: showStopper(); break;
    }
  }

//  delay(2000);
//  ss_esp.print("Hello");
//    ss_esp.write(9);
//  ss_esp.flush();
//  Serial.print("data flushed");
  
}


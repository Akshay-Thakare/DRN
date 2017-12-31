#include <Servo.h>

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
#define AIL 12
#define ELE 13
#define RUD 14
#define AUX 15
#define STP 20
//END Primary commands

Servo aux, rud, ele, ail, thr;
int current_aux=0, current_rud=0, current_ele=0, current_ail=0, current_thr=0;
 
void setup() {
  Serial.begin(9600);
 
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
 
void showStopper(){
  Serial.println("show stopper");
  aux.writeMicroseconds(0);
  rud.writeMicroseconds(0);
  ele.writeMicroseconds(0);
  ail.writeMicroseconds(0);
  thr.writeMicroseconds(0);
}
 
// TODO :: Print current values
 
void loop(){

  if(Serial.available()){
    char a[8];// = new char[10];
    int status = Serial.readBytesUntil('a', a, 7);
    if(status>5){
      int primary_cmd = (a[0]-'0')*10+(a[1]-'0');
      int val = (a[2]-'0')*1000+(a[3]-'0')*100+(a[4]-'0')*10+(a[5]-'0');
      
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
  }
  
}


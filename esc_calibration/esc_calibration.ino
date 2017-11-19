#include<Servo.h>

#define THROTTLE 5

#define THR_MIN 1150    // 0 Idle
#define THR_MAX 2390    // 157

Servo aux, rud, ele, ail, thr;
int current_aux=0, current_rud=0, current_ele=0, current_ail=0, current_thr=0;

void setup() {
  Serial.begin(9600);
  Serial.println("Program begin...");
  thr.attach(THROTTLE);
  thr.writeMicroseconds(0);     // MAX = FULL, MIN = m
}

void thr_max(){
  Serial.print("max");
  thr.writeMicroseconds(THR_MAX);
}

void thr_mid(){
  Serial.print("mid");
  thr.writeMicroseconds(THR_MAX/2);
}

void thr_min(){
  Serial.print("min");
  thr.writeMicroseconds(THR_MIN);
}

void loop() {
  if (Serial.available()){
    String m = Serial.readString();
    switch(m.charAt(0)){
      case '0': thr_max(); break;
      case '1': thr_mid(); break;
      case '2': thr_min(); break;
    }
  }
}

#include "scouting_car.h"
#include "Arduino.h"
#include <PPMReader.h>
#include <Wire.h>  
#include <WEMOS_Motor.h>
#include <Servo.h>
#include <iarduino_HC_SR04_tmr.h>
#include <iarduino_OLED.h>                                 
//#include "clicli.h"

car::car(int turret_pin){
  _turret_pin = turret_pin;
}

//creating motor driver obj
MotorShield MotorShield(MOTORSHIELD_AD11);

//creating servos
Servo turret;

//setting radio receiver up
  byte interruptPin =2;
  byte channelAmount = 6;
  PPMReader ppm(interruptPin, channelAmount);
    int old_value = 1;

iarduino_HC_SR04_tmr hc(10,9);

iarduino_OLED myOLED(0x3D);                               
extern uint8_t MediumFont[]; 

void car::begin(){
  pinMode(6, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial){
    ;
  }

  Serial.print("Connected");
  
  MotorShield.begin();
  
  hc.begin();
  
  turret.attach(_turret_pin);
  
  myOLED.begin();      
  myOLED.setFont(MediumFont);
  myOLED.print("Hi World!",0,14);  
  delay(2000);                                                          
  myOLED.clrScr();
  myOLED.print("Scouting",0,14); 
  myOLED.print("Robot: V1",0,35);
  delay(2000);                                                         
}

void car::run_motors(int gas, int steering, int dir){
  if (gas >=100){
    if (steering> 52){
      MotorShield.drive(dir*0.01*steering*gas,(100-steering)*0.01*gas*dir);
    }
    else if (steering < 48){
          MotorShield.drive(dir*0.01*steering*gas,(100-steering)*0.01*gas*dir);
    } 
    else {
          MotorShield.drive(dir*gas,gas*dir);
    }
  }
  else{
    MotorShield.drive(0,0);
  }
}
long car::get_gas(){
  unsigned gas = ppm.latestValidChannelValue(3, 500); // gas channel is 3. 
  return map(gas, 920, 2050, 0, 512);;
}

long car::get_steering(){
  unsigned steering_value = ppm.latestValidChannelValue(4, 500); // steering channel is 4.
  long steering =  map(steering_value, 1116, 1944, 0, 100);;
  return steering;
}

int car::get_direction(int gas, int old_value){
  unsigned dir_value = ppm.latestValidChannelValue(6, 500); // steering channel is 6.
  if (gas <= 100){
    if (dir_value > 1500){return 1;}
    else{return -1;}
  }
  else {
    return old_value;
  }
}

long car::get_turret_dir(){
  unsigned value = ppm.latestValidChannelValue(1, 500); 
  long dir = map(value,1144,1968,30,150);
  return dir;
}

void car::turn_turret(long turret_dir){
  turret.write(turret_dir); // value bigger than 90 turret left smaller than 90 turret right.  
}

void car::check_distance(){
  if (hc.distance()<20){
    myOLED.print("DISTANCE: ", 0,14);
    myOLED.print(hc.distance(), 0,35);
    myOLED.print("DANGER!!!", 0, 51);
  }
}

void car::run(){  
  myOLED.clrScr();
  long gas = get_gas();
  long steering = get_steering();
  long dir = get_direction(gas,old_value);
  long turret_dir = get_turret_dir();

  run_motors(gas ,steering, dir);
  turn_turret(turret_dir);
  check_distance();

  old_value = dir;
  
  Serial.print(gas);
  Serial.print('\t');
  Serial.print(steering);
  Serial.print('\t');
  Serial.print(dir);
  Serial.print('\t');
  Serial.print(turret_dir); 
  Serial.println();

  delay(20);
}

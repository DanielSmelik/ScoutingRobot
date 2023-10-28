#include "scouting_car.h" 
#include "Arduino.h"
#include <PPMReader.h>
#include <Wire.h>  
#include <WEMOS_Motor.h>
#include <Servo.h>
#include <iarduino_HC_SR04_tmr.h>
#include <iarduino_OLED.h>      
 //#include "clicli.h"

car::car(int turret_pin, int gun_pin, int led1_pin, int led2_pin){
  _turret_pin = turret_pin;
  _gun_pin = gun_pin;
  _led_pin1 = led1_pin;
  _led_pin2 = led2_pin;
  
}

//creating motor driver obj
MotorShield MotorShield(MOTORSHIELD_AD11);

//creating servos
Servo turret;
Servo gun; 

//setting radio receiver up
  byte interruptPin = 2;
  byte channelAmount = 6;
  PPMReader ppm(interruptPin, channelAmount);
    int old_value = 1;

//creating ultrasonic sensor
iarduino_HC_SR04_tmr hc(10,9);

//creating OLED. 
iarduino_OLED myOLED(0x3D);                               
extern uint8_t MediumFont[]; 

uint32_t old_time = 0;

void car::begin(){
  pinMode(_led_pin1, OUTPUT);
  pinMode(_led_pin2, OUTPUT);
  
  digitalWrite(_led_pin1, HIGH);
  digitalWrite(_led_pin2, HIGH);

  Serial.begin(115200);
  while (!Serial){
    ;
  }

  Serial.print("Connected");
  
  MotorShield.begin();
  
  hc.begin();
  
  turret.attach(_turret_pin);
  gun.attach(_gun_pin);

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
  // gets gas value and steering value, and gives each motor a steering percentage of gas and multiplys it by direcion.  
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
    else{
      digitalWrite(8, HIGH);
      digitalWrite(4, LOW);return -1;
      }
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

long car::get_gun_angle(){
  unsigned value = ppm.latestValidChannelValue(2, 500); 
  long gun_ang = map(value,1100,1920,30,150);
  return value;
}

void car::aim_gun(long gun_ang){
  gun.write(gun_ang);
}

void car::check_distance(){
  if (hc.distance()<20){
    myOLED.print("DISTANCE: ", 0,14);
    myOLED.print(hc.distance(), 0,35);
    myOLED.print("DANGER!!!", 0, 51);
  }
}

void car::blinkLEDS(){
  if (millis() - old_time >=750){
    digitalWrite(8, !digitalRead(8));
    digitalWrite(4, !digitalRead(4));
    old_time = millis();
    }
  }

void car::run(){  
  myOLED.clrScr();
  long gas = get_gas();
  long steering = get_steering();
  long dir = get_direction(gas,old_value);
  long turret_dir = get_turret_dir();
  long gun_ang = get_gun_angle();

  run_motors(gas ,steering, dir);
  turn_turret(turret_dir);
  aim_gun(gun_ang);
  check_distance();

  if (dir == -1){  
    blinkLEDS();
  }
  else{
    digitalWrite(8, HIGH);
    digitalWrite(4, HIGH);
  }

  old_value = dir;
  
  Serial.print(gas);
  Serial.print('\t');
  Serial.print(steering);
  Serial.print('\t');
  Serial.print(dir);
  Serial.print('\t');
  Serial.print(turret_dir); 
  Serial.print('\t');
  Serial.print(gun_ang);
  Serial.print('\t');
  Serial.print(old_time);
  Serial.println();
  delay(50);
}
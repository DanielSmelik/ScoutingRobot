#ifndef SCOUTING_CAR_H
#define SCOUTING_CAR_H

class car {

  private:
      int _turret_pin;
      int _gun_pin;
      int _led_pin1;
      int _led_pin2;

  public:
    car(int turret_pin, int gun_pin, int led1_pin, int led2_pin);
    
    //general.
    void begin();
    void run();
    void check_distance();
    void blinkLEDS();

    //Do action based on input value. 
    void run_motors(int gas, int steering, int dir);
    void aim_gun(long gun_ang);
    void turn_turret(long dir);

    //get value from radio. 
    long get_gas();
    long get_steering();
    int get_direction(int gas, int old_value);
    long get_turret_dir();;
    long get_gun_angle();
    


};

#endif 

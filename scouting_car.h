#ifndef SCOUTING_CAR_H
#define SCOUTING_CAR_H

class car {

  private:
      int _turret_pin;
      int _gun_pin;

  public:
    car(int turret_pin, int gun_pin);
    void begin();
    void run_motors(int gas, int steering, int dir);
    long get_gas();
    long get_steering();
    int get_direction(int gas, int old_value);
    long get_turret_dir();;
    void turn_turret(long dir);
    void run();
    void check_distance();
    long get_gun_angle();
    void aim_gun(long gun_ang);

};

#endif 

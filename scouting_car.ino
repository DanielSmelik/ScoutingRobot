#include "scouting_car.h"
//#include "clicli.h"

car mycar(3, 5, 4,8); 
//clicli mycli(car);  

void setup(){
  mycar.begin();
  }
  
void loop(){
  //mycli.run();
  mycar.run();
}

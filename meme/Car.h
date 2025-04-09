#ifndef CAR_H
#define CAR_H

#include <Arduino.h>
#define RED_LED 13
#define YELLOW_LED 12
#define GREEN_LED 11
class Car {

  private:
    int speed;
    int PIN_MOTOR;


  public:
    Car(int PIN_MOTOR);

    void Forward();
    void getSpeed();
    void setSpeed(int speed);
    void decSpeed();
    void incSpeed();
    void displaySpeed();

};

#endif
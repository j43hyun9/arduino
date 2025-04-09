// Car.cpp
#include "Car.h"


Car::Car(int PIN_MOTOR) {
  this->speed = 85;
  this->PIN_MOTOR = PIN_MOTOR;
  pinMode(PIN_MOTOR, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void Car::decSpeed() {
  Serial.println("dec");
  if (speed > 0) speed -= 85;
}

void Car::incSpeed() {
  Serial.println("inc");
  if (speed < 255) speed += 85;
}

void Car::Forward() {
  Serial.println("Forward");
  analogWrite(PIN_MOTOR, speed);
  delay(100);
  Serial.println("Stop");
  analogWrite(PIN_MOTOR, 0);
}

void Car::displaySpeed() {
  switch (speed) {
    case 85:
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      break;
    case 170:
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      break;
    case 255:
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      break;
  }
}

#include <IRremote.h>
#include <IRremoteInt.h>
#include <boarddefs.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include "Car.h"



int RECV_PIN = 2;
int MotorPin = 3;

IRrecv irrecv(RECV_PIN);
Car car(MotorPin);
decode_results results;

void forwardWrapper() {
  car.Forward();
}

void decSpeedWrapper() {
  car.decSpeed();
}

void incSpeedWrapper() {
  car.incSpeed();
}

void (*recentCall)() = nullptr;

void setRecentCaller(void (*func)()) {
  recentCall = func;
}

void recentCaller() {
  if (recentCall != nullptr) {
    recentCall();
  }
}
void setup()
{
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
}
//  F
void loop()
{

  car.displaySpeed();
  
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    switch (results.value) {
      case 0xFF6897: 
        car.Forward();  
        setRecentCaller(forwardWrapper); // 래퍼 함수 전달
        break;
      case 0xFFE01F: 
        car.decSpeed(); 
        setRecentCaller(decSpeedWrapper);
        break;
      case 0xFFA857: 
        car.incSpeed(); 
        setRecentCaller(incSpeedWrapper);
        break;
      case 0xFFFFFFFF: 
        recentCall(); // 저장된 함수 실행
        break;
    }
    irrecv.resume(); // Receive the next value
  }
  
}

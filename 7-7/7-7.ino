#include <Stepper.h>

const int STEPS = 2048;
Stepper stepper(STEPS, 8, 10, 9, 11);

void setup() {
  stepper.setSpeed(14);
  Serial.begin(9600);
  Serial.println("회전시킬 각도를 입력하시오.");

}

void loop() {
  if(Serial.available()) {
    int val=Serial.parseInt(); 
    val = map(val, -360, 360, -2048, 2048);
    stepper.step(val);
    Serial.println(val);
    delay(10);
  }

}

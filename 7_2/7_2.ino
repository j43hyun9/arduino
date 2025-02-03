#include <Servo.h>
Servo myServo;
void setup() {
  Serial.begin(9600);   // 시리얼 모니터와의 통신
  myServo.attach(12);

}

int val;
int angle;

void loop() {
  val = angalogRead(A0);
  angle = map(val, 0, 1023, 0, 180);
  myServo.write(angle);
  Serial.println(angle);
  delay(50);

}

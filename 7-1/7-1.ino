#include <Servo.h>

Servo myServo;  // 서보모터를 제어하기 위한 서보 객체의 생성
// 대부분의 보드에서 서보 객체를 12개까지 생성할 수 있다.
int motorPin = 12;
int pushPin = 7;
int angle = 90; // 서보 위치(각도)를 저장하기 위한 변수
int state = 0;
void setup() {
  myServo.attach(motorPin); // 핀 12번에 연결된 서보를 서보 객체에 배속시킴
  pinMode(pushPin, INPUT);
  Serial.begin(9600);
  Serial.println("Enter the push button.");

}

void loop() {
  if (digitalRead(pushPin) == HIGH) {
    if (state == 0) {
      angle += 1;
      if (angle>=180) state = 1;
      myServo.write(angle);
      delay(10);
      Serial.println(angle);
    }
    else {
      angle -= 1;
      if (angle<=0) state = 0;
      myServo.write(angle);
      delay(10);
      Serial.println(angle);
    }
  }

}

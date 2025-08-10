#include <Servo.h>
Servo myServo;

int motor = 12;
int angle = 90;
void setup() {
  myServo.attach(motor);
  Serial.begin(9600);
  Serial.println("Initial angle : 90 degrees");
  Serial.println("Enter U(u) or D(d)");
  Serial.println("U : Increase the angle by = degrees.");

}
void loop() {
  if(Serial.available())
  {
    char input = Serial.read();
    if(input == 'U' || input == 'u')
    {
      for(int i = 0; i < 5; i++)
      {
        angle += 1;
        if(angle >= 180) angle = 180;
        myServo.write(angle);
        delay(10);
      }
      Serial.println(angle);
    }
    else if(input == 'D' || input == 'd')
    {
      for(int i = 0; i < 5; i++)
      {
        angle -= 1;
        if(angle <= 0) angle = 0;
        myServo.write(angle);
        delay(10);
      }
      Serial.println(angle);
    }
    else if(input == '\n') {
    }
    else {
      Serial.println("wrong character");
    }
  }

}

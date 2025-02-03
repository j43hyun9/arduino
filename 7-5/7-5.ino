#include <Stepper.h>
const int stepsPerRevolution = 2048/8;

  Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);
void setup() {
  myStepper.setSpeed(120);
  Serial.begin(9600);

}

void loop() {
  if(Serial.available()) {
    myStepper.step(2048);
    Serial.println("clockwise");
    for( int x=1; x<9; x++)
    {
      myStepper.step(stepsPerRevolution);
      Serial.println(x);
    }
    delay(500);

    Serial.println("counterclockwise");
    for(int x=1; x<9; x++) 
    {
      myStepper.step(-stepsPerRevolution);
      Serial.println(x);
    }
    delay(500);
  } else {
    Serial.println("Serial available() is not");
  }

}

int led1 = 2, led2 = 4, led3 = 6;
int myLed[] = {led1, led2, led3};

void setup() {
  for(int i = 0; i < 3; i++)   {
    pinMode(myLed[i], OUTPUT);
  }
}

void loop() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(myLed[i], HIGH);
    delay(500);
    digitalWrite(myLed[i], LOW);
    delay(500);
  }
}

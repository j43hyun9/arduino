int buttonPin = 4; 
int ledPin = 13;
int buttonState = 0;

void setup() {
  // Q. 이 장에서 시리얼 통신을 사용하지 않는 이유는 출력을 led 로 확인가능하기 때문인가
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
  }
  else {
    digitalWrite(ledPin, LOW);
  }

}

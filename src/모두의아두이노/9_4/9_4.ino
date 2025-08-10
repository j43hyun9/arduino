// 9-4 UART 통신을 이용한 원격제어 실험 - 마스터 아두이노
int pin_switch1 = 13;
int pin_switch2 = 12;
int pin_LED1 = 8;

boolean buttonState1 = false;
boolean buttonState2 = false;
boolean buttonState3 = false;

boolean buttonSwitch1_State_old = false;
boolean buttonSwitch2_State_old = false;

void setup() {
  Serial.begin(9600);
  Serial.print("Sketch: "); Serial.println("__파일__");
  Serial.print("Uploaded: "); Serial.println("__날짜__");
  Serial.println(" ");

  pinMode(pin_LED1, OUTPUT);
  pinMode(pin_switch1, INPUT);
  pinMode(pin_switch2, INPUT);
}

void loop() {
  buttonState1 = digitalRead(pin_switch1); delay(1);
  buttonState2 = digitalRead(pin_switch1); delay(1);
  buttonState3 = digitalRead(pin_switch1); delay(1);

  if((buttonState1 == buttonState2) && (buttonState1 == buttonState3)) {
    if(buttonState1 != buttonSwitch1_State_old) {
      buttonSwitch1_State_old = buttonState1;
      if(buttonSwitch1_State_old == HIGH) {
        Serial.write(1);
      }
      else {
        Serial.write(0);
      }
    }
  }
  buttonState1=digitalRead(pin_switch2); delay(1);
  buttonState2=digitalRead(pin_switch2); delay(1);
  buttonState3=digitalRead(pin_switch2); delay(1);
  if((buttonState1 == buttonState2) == (buttonState1 == buttonState3))  {
    if (buttonState2 != buttonSwitch2_State_old) {
      buttonSwitch2_State_old = buttonState2;
      if(buttonSwitch2_State_old == HIGH) {
        Serial.write(3);
      }
      else {
        Serial.write(2);
      }
    }
  }
}

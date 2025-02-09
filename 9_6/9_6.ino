// UART 통신을 이용한 원격제어 실험 - 마스터 아두이노
int pin_switch1 = 13;
int pin_LED1 = 8;
int pin_LED2 = 9;
boolean buttonState1 = false;
boolean buttonSwitch1_State_old = LOW;
boolean buttonSwitch1_State_new = LOW;
boolean statusLED = LOW;
void setup() {
  Serial.begin(9600);
  pinMode(pin_LED1, OUTPUT);
  pinMode(pin_LED2, OUTPUT);
  digitalWrite(pin_LED1, LOW);
  digitalWrite(pin_LED2, LOW);
  pinMode(pin_switch1, INPUT);
}

void loop() {
  if(Serial.available()) {
    int received_data = Serial.read();
    if (received_data == 1) {
      digitalWrite(pin_LED2, HIGH); delay(500);
      digitalWrite(pin_LED2, LOW);
    }
  }
  buttonSwitch1_State_new=digitalRead(pin_switch1); delay(1);
  if(buttonSwitch1_State_new != buttonSwitch1_State_old) {
    if(buttonSwitch1_State_new == HIGH) {
      if(statusLED == LOW) {
        Serial.write(1);
        digitalWrite(pin_LED1, HIGH);
        statusLED = HIGH;
      }
      else {
        Serial.write(0);
        digitalWrite(pin_LED1, LOW);
        statusLED = LOW;
      }
    }
    buttonSwitch1_State_old=buttonSwitch1_State_new;
  }
}
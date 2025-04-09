int FirstPin = 5;
int Ireg_SerOut = 4;
//Pin connected to Serial_output of 74hc595
int serialOutPin_echo = 2;
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
// Pin connected to MR of 75HC595
int masterClear = 9; //
// Pin connected to OE of 75HC595
int outputDisable = 13; // active low
// Pin connected to LP 
int lastPin = 7;
int data[] = {0,0,0,0,0,0,0,1};
int data_size = sizeof(data) / sizeof(data[0]);
int echoSerial_arr[8] = {0,};
void setup() {
  // put your setup code here, to run once:
  pinMode(5, OUTPUT);
  pinMode(3, INPUT);
  pinMode(serialOutPin_echo, INPUT);
  pinMode(lastPin, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(masterClear, OUTPUT);
  pinMode(outputDisable, OUTPUT);

  digitalWrite(masterClear, LOW);
  digitalWrite(masterClear, HIGH);
  digitalWrite(outputDisable, LOW);
  //digitalWrite(latchPin, LOW);
  Serial.begin(9600);

  // Data Clear
  for(int i=0; i<data_size; i++) {
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
  }
  // Data Set
  for(int i=0; i<data_size; i++) {

  digitalWrite(dataPin, data[i]);
  //delay(10);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  }
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);

  //digitalWrite(outputDisable, HIGH); // active low
  //digitalWrite(outputDisable, LOW); // active low
   //delay(50);
  // digitalWrite(masterClear, HIGH);
  // digitalWrite(outputDisable, LOW); // active low
   //digitalWrite(dataPin, LOW); // reset

}
  
int cnt = 0;

// 디버깅시 clock = red 
void loop() {
  delay(100);
  // digitalWrite(masterClear, LOW);
  // // digitalWrite(masterClear, HIGH);
  // // digitalWrite(masterClear, LOW);

  for(int i=0; i<data_size; i++) {
    echoSerial_arr[i] = digitalRead(serialOutPin_echo);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
  }
  // digitalWrite(5, LOW);
  // delay(2);
  // digitalWrite(5, HIGH);
  // delay(10);
  // digitalWrite(5, LOW);
  // float len = pulseIn(3, HIGH);
  // 디버깅
  // Serial.print("Pin 4           : ");
  // Serial.println(digitalRead(4));
  // Serial.print("Length =        : ");
  // Serial.println(len);
  // Serial.print("C (A0)          : ");
  // Serial.println(analogRead(A0));
  // Serial.print("A (serialOutPin): "); // Arduino(Pin 9)
  // Serial.println(digitalRead(serialOutPin_echo));
  // Serial.print("B  (lastPin)    : "); // Q7
  // Serial.println(digitalRead(lastPin));
  Serial.print("serialPin_arr = [ ");
  for(int i=0; i<data_size; i++) {
    Serial.print(echoSerial_arr[i]);
    if(i != data_size -1) {
      Serial.print(", ");
    } else {
      Serial.print(" ");
    }
  }
  Serial.println("]");

  // int cycleData = 0;
  int ReadToLastPin = digitalRead(lastPin);
  int ReadToDataPin = digitalRead(dataPin);
  int ReadToFirstPin = digitalRead(FirstPin);
  int ReadToIreg_SerPin = digitalRead(Ireg_SerOut);
  float ReadToEcho = 0;
  if(ReadToLastPin == HIGH){
    // cycleData = 1;
    // digitalWrite(dataPin, LOW);
    // delay(2);
    // digitalWrite(dataPin, HIGH);
    // delay(10);
    // digitalWrite(dataPin, LOW);
    // // testlen = pulseIn(3, HIGH);
    // delay(10);
    digitalWrite(dataPin, HIGH);
  }
  else{
    digitalWrite(dataPin, LOW);
  }
  ReadToDataPin = digitalRead(dataPin); // set current Data

  digitalWrite(clockPin, HIGH);
  delay(500);
  digitalWrite(clockPin, LOW);
  delay(500);
  digitalWrite(latchPin, HIGH);
  //delay(500);
  digitalWrite(latchPin, LOW);

  //digitalWrite(dataPin, LOW); // ReadToLastPin == HIGH, after Pulse on state LOW
  ReadToEcho = pulseIn(3, HIGH);
  float Length = ((float)(340 * ReadToEcho) / 10000 ) / 2;
  
  Serial.println("-----------Post- Pulse-----------");
  Serial.print("                   IregOut         : ");
  Serial.println(ReadToIreg_SerPin);
  Serial.print("                   First         : ");
  Serial.println(ReadToFirstPin);
  Serial.print("                   Data          : ");
  Serial.println(ReadToDataPin);
  Serial.print("                   Last          : ");
  Serial.println(ReadToLastPin);
  Serial.print("                    → Length          : ");
  Serial.print(Length);
  Serial.println("cm");
  
  

}

void debug(int pin) {
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
    delay(1000);
}

// 조도
void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(A0);  // 아날로그 값 읽기
  Serial.println(sensorValue);
  if (sensorValue > 10) digitalWrite(4, LOW);
  else digitalWrite(4, HIGH);
  delay(200);

}

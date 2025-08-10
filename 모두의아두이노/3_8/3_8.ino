// LM35 온도 측정
int Pin = A0; // 입력 핀
int reading;
float tempC;

void setup() {
  Serial.begin(9600);
}

void loop() {
  reading = analogRead(Pin);   //  센서 값을 읽어옴
  tempC = (5.0*reading*100.0)/1024.0;
  Serial.print(tempC);  // 센서 값을 섭씨온도로 변환( LM35 계산 공식에 의함)
  Serial.println(" 도");
  delay(1000);  // 온도 측정 간격 설정 1초
}

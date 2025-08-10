void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int LED_ON = 1;
  int LED_OFF = 0;
  Serial.write(LED_ON);
  Serial.println(LED_ON);
  delay(1000);
  Serial.write(LED_OFF);
  Serial.println(LED_OFF);
  delay(1000);
    
  

}

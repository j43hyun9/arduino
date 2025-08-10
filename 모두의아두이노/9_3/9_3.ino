void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    int received_data = Serial.read();
    Serial.println(received_data);
    if (received_data == 1){
      digitalWrite(13, HIGH);
    }
    else {
      digitalWrite(13, LOW);
    }
    
  }

}

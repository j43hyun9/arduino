int ledPin = 13;
int val = 0;
void setup() {
  pinMode(ledPin, OUTPUT);
}     

void loop() {
  val = analogRead(A0);
  digitalWrite(ledPin, HIGH);
  delay(val);
  digitalWrite(ledPin, LOW);
  delay(val);
}

byte col = 0;
byte leds[8][8];

int pins[17]= {-1, 5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};

int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};

int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};



void setup() {
  // put your setup code here, to run once:
  for( int i=1; i<=16; i++) {
    pinMode(pins[i], OUTPUT);
  }
  for( int i=1; i<=8; i++) {
    digitalWrite(cols[i-1], HIGH);
  }
  for( int i=1; i<=8; i++) {
    digitalWrite(rows[i-1], LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(rows[1], HIGH);
  digitalWrite(cols[1], LOW);
  digitalWrite(cols[2], LOW);
}

#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);    // RX, TX

void setup() {
  // pinMode(4, OUTPUT);
  // pinMode(rxPin, INPUT);
  // pinMode(txPin, OUTPUT);

  // Serial.begin(9600);
  // mySerial.begin(9600);
  mySerial.begin(9600);
  pinMode(4, OUTPUT);
}

void loop() {
  // String m = mySerial.readString();
//   if (mySerial.available() > 0) {
//         char c = mySerial.read();
// //        String m = mySerial.read();
//         String m = mySerial.readString();
//         Serial.print("c:");
//         Serial.println(c);
//         Serial.print("m:");
//         Serial.println(m);
//         delay(100000);
//     }
  // if (Serial.available()) mySerial.write(Serial.read());
  // if (mySerial.available()) Serial.write(mySerial.read());

if(mySerial.available() > 0) {
  String m = mySerial.readString();

  if (m.indexOf("1") == 0) digitalWrite(4, HIGH);
  if(m.indexOf("0") == 0)  digitalWrite(4, LOW);
}



  
  
  

  //mySerial.println(c);
  //delay(10000);

  //Serial.print(m);
  // int readValue_A0 = analogRead(A0);
  // int readValue_A1 = analogRead(A1);

  // Serial.print("RX is A0: ");
  // Serial.println(readValue_A0);
  // delay(1000);
  // Serial.print("TX is A1: ");
  // Serial.println(readValue_A1);
  // delay(1000);
  // if(Serial.available()) {
  //   mySerial.write(Serial.read());
  //  }
  // if(mySerial.available()) {
  //   //Serial.print("mySerial is available");
  //   Serial.write(mySerial.read());
  //   Serial.print("mySerial");
  //   delay(10000);
  // }
  // if (mySerial.available() > 0) {
    
  //   //Serial.print(m);
  //     // 문자열로 읽은 값을 m에 저장
  //   if (m.indexOf("1") == 0) digitalWrite(4, HIGH);
  //       // '1' 이면 HIGH 신호 보냄(LED 켬)
  //   //else Serial.print("not indexOf 1");
  //   if (m.indexOf("0") == 0) digitalWrite(4, LOW);
  //       // '0' 이면 LOW 신호 보냄(LED 끔)
  //   //else Serial.print("not indexOf 0");
  // // } else {
  // //   //Serial.println("mySerial is disable");
  //  }

  // if(mySerial.available()) {
  //   if (m.indexOf("1") == 0) digitalWrite(4, HIGH);
  //   if (m.indexOf("0") == 0) digitalWrite(4, LOW);
  //   //Serial.print("test");
//}
  

  
  

}

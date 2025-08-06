#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SPI.h>
#include <MFRC522.h>

/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */
// branch symbol
#define __MAIN__ 1
#define __SELECT__ 1


/*RC522*/
#define SS_PIN 10
#define RST_PIN 9
SoftwareSerial mySerial(6, 7); // RX, TX
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
const int NUM_TAGS = 32;
const int UID_LEN = 4;
const int REGISTER = 2;
const int LISTEN = 1;
byte tagUIDs[NUM_TAGS][4] = {};


/*
모드
1 -> 읽기모드
2 -> 쓰기모드
3 -> 등록모드

동작
등록성공 2번 깜빡임
등록실패 5번 깜빡임
*/
int currentMod=3;

int readPin = 2;
int writePin = 3;

int readInput = 4;
int writeInput = 5;
int busyPin = 8;
void setup() { 
  admin_SetUp();
  mf_Setup();
  df_Setup();
}
void admin_SetUp() {
  pinMode(readPin, OUTPUT);
  pinMode(writePin, OUTPUT);
  pinMode(readInput, INPUT);
  pinMode(writeInput, INPUT);
  pinMode(busyPin, INPUT);
  digitalWrite(readPin, HIGH);
  digitalWrite(writePin, LOW);
}
void df_Setup() {
	mySerial.begin (9600);
	mp3_set_serial (mySerial);	//set softwareSerial for DFPlayer-mini mp3 module 
	mp3_set_volume (15);
}
void mf_Setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  // ID값 초기화 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("이 코드는 MIFARE 클래식 NUID를 스캔합니다."));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {
  // 읽기모드, 쓰기모드
  // 읽기모드에서는 입력한 태그값이 들어있는 인덱스 번호 파일을 재생시켜줌
  // 쓰기모드에서는 입력한 인덱스값에 태그값을 저장함
  // 읽기모드는 붉은 LED
  // 쓰기모드는 노란 LED 각각 스위치
  // 쓰기모드 상태에서 붉은색 LED는 인덱스를 의미함 홀수일 경우 꺼짐 짝수일 경우 켜짐
  // 인덱스를 선택한 뒤 노란 LED 스위치를 한번 더 누름 노란불, 빨간불 둘다 켜진다면 등록모드임
  // 등록모드 상태에서 태그값 입력이 없을 경우 종료됨. 종료 모션은 LED 2개 3번깜빡임
  // 종료된 이후에는 읽기모드로 돌아감
  
  // while(__MAIN__) {
    while(__SELECT__) {
      int readState = digitalRead(readInput);
      int writeState = digitalRead(writeInput);
      if(readState == HIGH && writeState == LOW) {
        Serial.println(">> 1. 대기모드 버튼을 누르셨습니다.");
        mp3_stop();
        currentMod = LISTEN;
        delay(3000);
        break;
      } else if (readState == LOW && writeState == HIGH) {
        Serial.println(">> 2. 등록모드 버튼을 누르셨습니다.");
        currentMod = REGISTER;
        delay(3000);
        break;
      } else if (readState == HIGH && writeState == HIGH) {
        Serial.println(">> 3. 모든 태그값을 초기화합니다.");
        digitalWrite(readPin, LOW);
        digitalWrite(writePin, LOW);
        delay(300);
        digitalWrite(readPin, HIGH);
        digitalWrite(writePin, HIGH);
        delay(300);
        digitalWrite(readPin, LOW);
        digitalWrite(writePin, LOW);
        delay(300);
        digitalWrite(readPin, HIGH);
        digitalWrite(writePin, HIGH);
        delay(300);
        digitalWrite(readPin, LOW);
        digitalWrite(writePin, LOW);
        delay(300);
        digitalWrite(readPin, HIGH);
        digitalWrite(writePin, HIGH);
        delay(300);
        for (int i = 0; i < NUM_TAGS; i++) {
          for (int j = 0; j < UID_LEN; j++) {
            tagUIDs[i][j] = 0;
          }
        }
        delay(3000);
        break;
      } else {
        Serial.print("else ");  
      }
      
      // 기본 값은 대기모드
      Serial.print("currentMod : ");
      Serial.println(currentMod);
      currentMod = LISTEN;
      break;
      
    }

    Serial.print("Ex) currentMod : ");
    Serial.println(currentMod);
    Serial.print("Ex) register : ");
    Serial.println(REGISTER);
    Serial.print("Ex) type(currentMod) : ");
    Serial.println(sizeof(currentMod));
    Serial.print("Ex) type(register) : ");
    Serial.println(sizeof(REGISTER));
    
  
  
    if( currentMod == LISTEN ) {
        Serial.println(F(">> 대기모드 로직 시작"));
        digitalWrite(readPin, HIGH);
        digitalWrite(writePin, LOW);
        // 새 카드 접촉이 있을 때만 다음 단계로 넘어간다.
        if ( ! rfid.PICC_IsNewCardPresent())
          return;
        // Verify if the NUID has been readed
        if ( ! rfid.PICC_ReadCardSerial())
          return;
        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        Serial.println(rfid.PICC_GetTypeName(piccType));
        // Check is the PICC of Classic MIFARE type
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
          Serial.println(F("Your tag is not of type MIFARE Classic."));
          return;
        }
        bool play_state = digitalRead(busyPin);
        if(play_state != HIGH) {
          Serial.println(F("음악 재생이 끝나지 않았습니다."));
          return;
          } 
        int idx = findTagIndex(rfid.uid.uidByte, tagUIDs, NUM_TAGS);
      
        if (idx != -1) {
          Serial.print("태그가 배열에 있습니다. 인덱스 = ");
          Serial.println(idx);
          Serial.print("해당 인덱스에 대해 음악 재생");
          delay(300);
          mp3_play(idx);
        } else {
          Serial.println("배열에 해당 태그가 없습니다.");
        }
        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        }
    else if ( currentMod == REGISTER ) {
        Serial.println(F(">> 등록모드 활성화"));
        Serial.println(F("  >> RED LED : Index Inc"));
        Serial.println(F("  >> BLUE LED : Register"));
        digitalWrite(readPin, LOW);
        digitalWrite(writePin, HIGH);
        delay(300);
        int temp = 100; // default 값
        int index = 0;
        while(true) {
          int indexInc = digitalRead(readInput);
          int setRegister = digitalRead(writeInput);
          if(setRegister == HIGH) {
            Serial.println(F(">> 등록할 태그키를 리더기에 붙여주세요."));
            Serial.print(F(">> target 음원 파일번호 : "));
            Serial.println(index);
            unsigned long registerStartTime = millis(); // 등록모드 진입 시점 저장
            while(true) {
              // 10초 초과 시 종료
              if (millis() - registerStartTime > 10000) {
                Serial.println("등록시간 초과, 등록모드 종료.");
                break;
              }
              if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
                continue;

              // UID 중복 체크 후 저장
              if (!isRegistered(rfid.uid.uidByte)) {
                // 신규 태그라면 저장
                for (int i = 0; i < UID_LEN; i++) {
                  tagUIDs[index][i] = rfid.uid.uidByte[i];
                }
                Serial.print("새로운 태그 변경: ");
                for (int i = 0; i < UID_LEN; i++) {
                  Serial.print(tagUIDs[index][i], HEX);
                  Serial.print(" ");
                }
                Serial.println();
              } else {
                Serial.println("이미 등록된 태그입니다");
                return;
              }
                Serial.println();
                digitalWrite(writePin, LOW);
                delay(300);
                digitalWrite(writePin, HIGH);
                delay(300);
                digitalWrite(writePin, LOW);
                delay(300);
                digitalWrite(writePin, HIGH);
                delay(300);
                digitalWrite(writePin, LOW);
                delay(300);
                digitalWrite(writePin, HIGH);
                delay(300);
                return;
            }
            Serial.println(F("타임아웃, "));
            delay(10000);
            break;
          } else if(indexInc == HIGH) {
                    if(temp != indexInc) {
                      if(temp != indexInc) {
                          Serial.print(F("index : "));
                          Serial.println(++index);
                          index = index%32;
                          if(index % 2 == 1) {
                            digitalWrite(readPin, LOW);
                          } else if (index%2 == 0) {
                            digitalWrite(readPin, HIGH);
                          }
                          if(index == 0) {
                            index = 0;
                            digitalWrite(readPin, LOW);
                            delay(300);
                            digitalWrite(readPin, HIGH);
                            delay(300);
                            digitalWrite(readPin, LOW);
                            delay(300);
                            digitalWrite(readPin, HIGH);
                            delay(300);
                            digitalWrite(readPin, LOW);
                            delay(300);
                            digitalWrite(readPin, HIGH);
                            delay(300);
                          }
                      }
                    } 
                  }
            temp = indexInc; // temp에 n_read의 이전값을 저장한다.
      }
    }
  
  Serial.print("Ox) currentMod : ");
  Serial.println(currentMod);


  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  // }
}

// UID 중복 여부 확인 함수
bool isRegistered(byte* uid) {
  for (int i = 0; i < NUM_TAGS; i++) {
    bool matched = true;
    for (int j = 0; j < UID_LEN; j++) {
      if (tagUIDs[i][j] != uid[j]) {
        matched = false;
        break;
      }
    }
    if (matched) return true;
  }
  return false;
}
/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */



void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}
bool compareUID(byte *a, byte *b, int len) {
  for (int i = 0; i < len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}
 // 배열 내에서 태그(UID)의 인덱스 찾기 함수
int findTagIndex(byte *uid, byte tagArray[][UID_LEN], int arrLen) {
  for (int i = 0; i < arrLen; i++) {
    if (compareUID(uid, tagArray[i], UID_LEN)) {
      return i; // 일치하는 인덱스 반환
    }
  }
  return -1; // 찾지 못하면 -1 반환
}

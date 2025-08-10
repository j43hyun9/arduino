#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>  // EEPROM 라이브러리 추가

#define SS_PIN 10
#define RST_PIN 9

SoftwareSerial mySerial(6, 7); // RX, TX

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const int NUM_TAGS = 32;
const int UID_LEN = 4;

byte tagUIDs[NUM_TAGS][UID_LEN] = {};

#define REGISTER 2
#define LISTEN 1

int currentMod = 3;

int readPin = 2;
int writePin = 3;

int readInput = 4;
int writeInput = 5;
int busyPin = 8;

#define EEPROM_SONG_INDEX_ADDR 0 // EEPROM에 노래 인덱스 저장 위치 (2바이트)

// EEPROM에 노래 인덱스 저장 함수 (int → 2바이트)
void saveSongIndexToEEPROM(int index) {
  EEPROM.update(EEPROM_SONG_INDEX_ADDR, index & 0xFF);
  EEPROM.update(EEPROM_SONG_INDEX_ADDR + 1, (index >> 8) & 0xFF);
}

// EEPROM에서 노래 인덱스 읽기 함수
int readSongIndexFromEEPROM() {
  int lowByte = EEPROM.read(EEPROM_SONG_INDEX_ADDR);
  int highByte = EEPROM.read(EEPROM_SONG_INDEX_ADDR + 1);
  return (highByte << 8) | lowByte;
}

// UID 비교 함수
bool compareUID(byte *a, byte *b, int len) {
  for (int i = 0; i < len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// 태그 배열에서 UID 인덱스 찾기
int findTagIndex(byte *uid, byte tagArray[][UID_LEN], int arrLen) {
  for (int i = 0; i < arrLen; i++) {
    if (compareUID(uid, tagArray[i], UID_LEN)) {
      return i; // 찾으면 인덱스 리턴
    }
  }
  return -1; // 없으면 -1
}

// 등록 여부 확인 함수
bool isRegistered(byte* uid) {
  for (int i = 0; i < NUM_TAGS; i++) {
    if (compareUID(uid, tagUIDs[i], UID_LEN)) {
      return true;
    }
  }
  return false;
}

// 16진수 출력 헬퍼
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// 10진수 출력 헬퍼
void printDec(byte *buffer, byte bufferSize) {
  for(byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
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
  mp3_set_serial (mySerial);  // DFPlayer-Mini mp3 소프트웨어 시리얼 설정
  mp3_set_volume (15);
}

void mf_Setup() {
  Serial.begin(9600);
  SPI.begin(); // SPI 버스 초기화
  rfid.PCD_Init(); // RFID 리더 초기화

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("이 코드는 MIFARE 클래식 NUID를 스캔합니다."));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loadTagUIDsFromEEPROM() {
  int addr = 2; // 0,1번지는 노래 인덱스 저장용으로 비워둠
  for(int i = 0; i < NUM_TAGS; i++) {
    for(int j = 0; j < UID_LEN; j++) {
      tagUIDs[i][j] = EEPROM.read(addr++);
    }
  }
  Serial.println("EEPROM에서 tagUIDs 복원 완료");
}

void saveTagUIDsToEEPROM() {
  int addr = 2; // 0,1번지는 노래 인덱스 저장 공간
  for(int i = 0; i < NUM_TAGS; i++) {
    for(int j = 0; j < UID_LEN; j++) {
      EEPROM.update(addr++, tagUIDs[i][j]);
    }
  }
  Serial.println("EEPROM에 tagUIDs 저장 완료");
}

void setup() {
  admin_SetUp();
  mf_Setup();
  df_Setup();

  // EEPROM에서 태그UID 배열 복원
  loadTagUIDsFromEEPROM();

  // EEPROM에 저장된 노래 인덱스 읽어오기
  int savedIdx = readSongIndexFromEEPROM();
  Serial.print("EEPROM에서 읽은 저장된 노래 인덱스: ");
  Serial.println(savedIdx);
}

void loop() {
  while(true) {
    int readState = digitalRead(readInput);
    int writeState = digitalRead(writeInput);

    if(readState == HIGH && writeState == LOW) {
      Serial.println(">> 1. 대기모드 버튼을 누르셨습니다.");
      mp3_stop();
      currentMod = LISTEN;
      delay(3000);
      break;
    } else if(writeState == HIGH && readState == LOW) {
      Serial.println(">> 2. 등록모드 버튼을 누르셨습니다.");
      currentMod = REGISTER;
      delay(3000);
      break;
    } else if(readState == HIGH && writeState == HIGH) {
      Serial.println(">> 3. 모든 태그값 초기화");
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

      for(int i = 0; i < NUM_TAGS; i++) {
        for(int j = 0; j < UID_LEN; j++) {
          tagUIDs[i][j] = 0;
        }
      }

      // EEPROM에도 초기화된 tagUIDs 저장
      saveTagUIDsToEEPROM();

      delay(3000);
      break;
    } else {
      currentMod = LISTEN;
      break;
    }
  }

  if(currentMod == LISTEN) {
    digitalWrite(readPin, HIGH);
    digitalWrite(writePin, LOW);

    if(!rfid.PICC_IsNewCardPresent()) return;
    if(!rfid.PICC_ReadCardSerial()) return;

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    if(piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
       piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
       piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }

    bool play_state = digitalRead(busyPin);
    if(play_state != HIGH) {
      Serial.println(F("음악 재생이 끝나지 않았습니다."));
      return;
    }

    int idx = findTagIndex(rfid.uid.uidByte, tagUIDs, NUM_TAGS);

    if(idx != -1) {
      Serial.print("태그가 배열에 있습니다. 인덱스 = ");
      Serial.println(idx);
      Serial.print("해당 인덱스에 대해 음악 재생");
      delay(300);

      // 노래 인덱스 EEPROM에 저장
      saveSongIndexToEEPROM(idx);

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
  else if(currentMod == REGISTER) {
    Serial.println(F(">> 등록모드 활성화"));
    Serial.println(F("  >> RED LED : Index Inc"));
    Serial.println(F("  >> BLUE LED : Register"));
    digitalWrite(readPin, LOW);
    digitalWrite(writePin, HIGH);
    delay(300);

    int temp = 100; // 임시값
    int index = 0;

    while(true) {
      int indexInc = digitalRead(readInput);
      int setRegister = digitalRead(writeInput);

      if(setRegister == HIGH) {
        Serial.println(F(">> 등록할 태그키를 리더기에 붙여주세요."));
        Serial.print(F(">> target 음원 파일번호 : "));
        Serial.println(index);
        
        unsigned long registerStartTime = millis();

        while(true) {
          if(millis() - registerStartTime > 10000) {
            Serial.println("등록시간 초과, 등록모드 종료.");
            break;
          }
          if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
            continue;

          if(!isRegistered(rfid.uid.uidByte)) {
            for(int i=0; i < UID_LEN; i++) {
              tagUIDs[index][i] = rfid.uid.uidByte[i];
            }
            Serial.print("새로운 태그 저장: ");
            for(int i = 0; i < UID_LEN; i++) {
              Serial.print(tagUIDs[index][i], HEX);
              Serial.print(" ");
            }
            Serial.println();

            // 변경된 tagUIDs EEPROM에 저장
            saveTagUIDsToEEPROM();

          } else {
            Serial.println("이미 등록된 태그입니다");
            return;
          }

          // 등록 완료 표시 (파란 LED 깜빡임)
          for(int i=0; i<3; i++) {
            digitalWrite(writePin, LOW);
            delay(300);
            digitalWrite(writePin, HIGH);
            delay(300);
          }

          return; // 등록 완료 후 함수 종료
        }
        Serial.println(F("타임아웃"));
        delay(10000);
        break;
      } else if(indexInc == HIGH) {
        if(temp != indexInc) {
          Serial.print(F("index : "));
          index = (index + 1) % NUM_TAGS;

          Serial.println(index);

          if(index % 2 == 1) {
            digitalWrite(readPin, LOW);
          } else {
            digitalWrite(readPin, HIGH);
          }

          if(index == 0) {
            for(int i=0; i<3; i++) {
              digitalWrite(readPin, LOW);
              delay(300);
              digitalWrite(readPin, HIGH);
              delay(300);
            }
          }
        }
      }
      temp = indexInc;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

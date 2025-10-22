#include <SoftwareSerial.h>
#include <DFMiniMp3.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9

// DFPlayer 콜백 클래스
class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action) {
    if (source & DfMp3_PlaySources_Sd) Serial.print("SD Card, ");
    Serial.println(action);
  }
  static void OnError(DFMiniMp3<SoftwareSerial, Mp3Notify>& mp3, uint16_t errorCode) {
    Serial.print("DFPlayer Error: ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DFMiniMp3<SoftwareSerial, Mp3Notify>& mp3, DfMp3_PlaySources source, uint16_t track) {
    Serial.print("Play finished: ");
    Serial.println(track);
  }
  static void OnPlaySourceOnline(DFMiniMp3<SoftwareSerial, Mp3Notify>& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DFMiniMp3<SoftwareSerial, Mp3Notify>& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DFMiniMp3<SoftwareSerial, Mp3Notify>& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "removed");
  }
};

SoftwareSerial dfSerial(6, 7); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(dfSerial);

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

const int NUM_TAGS = 32;
const int UID_LEN = 4;

byte tagUIDs[NUM_TAGS][UID_LEN] = {};  // Key: 태그 UID
int songIndices[NUM_TAGS] = {};        // Value: 노래 인덱스 (0~31)

#define REGISTER 2
#define LISTEN 1

int currentMod = 3;

int readPin = 2;
int writePin = 3;

int readInput = 4;
int writeInput = 5;
int busyPin = A2;        // DFPlayer 재생 상태 확인

int volumeUpPin = A0;    // 음량 증가 버튼
int volumeDownPin = A1;  // 음량 감소 버튼

#define EEPROM_LAST_SONG_ADDR 0    // EEPROM에 마지막 재생 노래 저장 위치 (2바이트: 0-1)
#define EEPROM_TAG_START_ADDR 2    // 태그 데이터 시작 주소
#define EEPROM_VOLUME_ADDR 194     // EEPROM에 음량 저장 위치 (1바이트: 194)
// 메모리 맵:
// 0-1: 마지막 재생 노래 (2바이트)
// 2-129: 태그 UID (32개×4바이트 = 128바이트)
// 130-193: 노래 인덱스 (32개×2바이트 = 64바이트)
// 194: 볼륨 (1바이트)

int currentVolume = 15;  // 현재 음량 (0~30)

// 음성 안내 파일 인덱스 (SD카드 파일명 기준)
#define VOICE_POWER_ON 0      // 0001.mp3 - 전원이 켜졌습니다
#define VOICE_REGISTER_MODE 1 // 0002.mp3 - 등록모드
#define VOICE_LISTEN_MODE 2   // 0003.mp3 - 읽기모드
#define VOICE_BRING_TAG 3     // 0004.mp3 - 태그키를 가져다대세요
#define VOICE_REGISTER_DONE 4 // 0005.mp3 - 등록완료되었습니다

// 숫자 음성 파일 (0006.mp3~0015.mp3 = 영, 일, 이, 삼, 사, 오, 육, 칠, 팔, 구)
#define VOICE_NUMBER_START 5  // 0006.mp3부터 시작 (인덱스 5)

// 실제 노래는 16번(0016.mp3)부터 시작 (인덱스 0 = 0016.mp3)
// trackNumber = idx + SONG_START_INDEX + 2 이므로 0 + 15 + 2 = 17
#define SONG_START_INDEX 14

// EEPROM에 마지막 재생 노래 저장 함수 (int → 2바이트)
void saveLastSongToEEPROM(int songIndex) {
  EEPROM.update(EEPROM_LAST_SONG_ADDR, songIndex & 0xFF);
  EEPROM.update(EEPROM_LAST_SONG_ADDR + 1, (songIndex >> 8) & 0xFF);
}

// EEPROM에서 마지막 재생 노래 읽기 함수
int readLastSongFromEEPROM() {
  int lowByte = EEPROM.read(EEPROM_LAST_SONG_ADDR);
  int highByte = EEPROM.read(EEPROM_LAST_SONG_ADDR + 1);
  return (highByte << 8) | lowByte;
}

// EEPROM에 음량 저장 함수
void saveVolumeToEEPROM(int volume) {
  EEPROM.update(EEPROM_VOLUME_ADDR, volume);
}

// EEPROM에서 음량 읽기 함수
int readVolumeFromEEPROM() {
  int volume = EEPROM.read(EEPROM_VOLUME_ADDR);
  // 유효하지 않은 값이면 기본값 15 반환 후 EEPROM에 저장
  if(volume > 30 || volume == 255) {  // 255는 초기화되지 않은 EEPROM 값
    volume = 15;
    EEPROM.update(EEPROM_VOLUME_ADDR, volume);  // 기본값 저장
  }
  return volume;
}

// UID 비교 함수
bool compareUID(byte *a, byte *b, int len) {
  for (int i = 0; i < len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// 태그 배열에서 UID가 저장된 슬롯 찾기
int findTagSlot(byte *uid, byte tagArray[][UID_LEN], int arrLen) {
  for (int i = 0; i < arrLen; i++) {
    if (compareUID(uid, tagArray[i], UID_LEN)) {
      return i; // 태그가 저장된 슬롯 번호 반환
    }
  }
  return -1; // 없으면 -1
}

// 빈 슬롯 찾기 (새 태그 등록 시 사용)
int findEmptySlot() {
  for (int i = 0; i < NUM_TAGS; i++) {
    // 모든 바이트가 0이거나 255인 경우 빈 슬롯
    bool isEmpty = true;
    for (int j = 0; j < UID_LEN; j++) {
      if (tagUIDs[i][j] != 0 && tagUIDs[i][j] != 255) {
        isEmpty = false;
        break;
      }
    }
    if (isEmpty) return i;
  }
  return -1; // 빈 슬롯 없음
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

// 인덱스 번호를 음성으로 재생하는 함수
void playIndexVoice(int index) {
  // 0~31까지의 인덱스를 십의 자리와 일의 자리로 분리하여 재생
  int tens = index / 10;  // 십의 자리
  int ones = index % 10;  // 일의 자리

  if(tens > 0) {
    // 십의 자리 재생 (1, 2, 3)
    mp3.playMp3FolderTrack(VOICE_NUMBER_START + tens + 1);
    delay(800); // 음성 재생 대기
  }

  // 일의 자리 재생 (0~9)
  mp3.playMp3FolderTrack(VOICE_NUMBER_START + ones + 1);
  delay(800); // 음성 재생 대기
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
  pinMode(readInput, INPUT_PULLUP);   // 내장 풀업 저항 활성화
  pinMode(writeInput, INPUT_PULLUP);  // 내장 풀업 저항 활성화
  pinMode(busyPin, INPUT);
  pinMode(volumeUpPin, INPUT_PULLUP); // 음량 버튼도 풀업으로
  pinMode(volumeDownPin, INPUT_PULLUP);
  digitalWrite(readPin, HIGH);
  digitalWrite(writePin, LOW);
}

void df_Setup() {
  mp3.begin();

  // EEPROM에서 저장된 음량 불러오기
  currentVolume = readVolumeFromEEPROM();
  mp3.setVolume(currentVolume);

  Serial.print("현재 음량: ");
  Serial.println(currentVolume);

  delay(500); // DFPlayer 초기화 대기
}

void mf_Setup() {
  Serial.begin(9600);
  SPI.begin(); // SPI 버스 초기화
  rfid.PCD_Init(); // RFID 리더 초기화

  // 안테나 게인 설정 (신호 감도 향상)
  rfid.PCD_SetAntennaGain(rfid.RxGain_max); // 최대 감도로 설정

  // RFID 모듈 자체 테스트
  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print(F("MFRC522 버전: 0x"));
  Serial.println(version, HEX);
  if(version == 0x00 || version == 0xFF) {
    Serial.println(F("경고: MFRC522 통신 실패! 배선을 확인하세요."));
  }

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("이 코드는 MIFARE 클래식 NUID를 스캔합니다."));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loadTagDataFromEEPROM() {
  int addr = EEPROM_TAG_START_ADDR;

  // UID 읽기
  for(int i = 0; i < NUM_TAGS; i++) {
    for(int j = 0; j < UID_LEN; j++) {
      tagUIDs[i][j] = EEPROM.read(addr++);
    }
  }

  // 노래 인덱스 읽기 (addr은 현재 2 + 32*4 = 130)
  for(int i = 0; i < NUM_TAGS; i++) {
    int lowByte = EEPROM.read(addr++);
    int highByte = EEPROM.read(addr++);
    songIndices[i] = (highByte << 8) | lowByte;
  }

  Serial.println("EEPROM에서 태그 데이터 복원 완료");

  // 디버깅: 저장된 태그 출력
  Serial.println("=== 저장된 태그 목록 ===");
  int tagCount = 0;
  for(int i = 0; i < NUM_TAGS; i++) {
    // 빈 슬롯 체크
    bool isEmpty = true;
    for(int j = 0; j < UID_LEN; j++) {
      if(tagUIDs[i][j] != 0 && tagUIDs[i][j] != 255) {
        isEmpty = false;
        break;
      }
    }

    if(!isEmpty) {
      tagCount++;
      Serial.print("슬롯 ");
      Serial.print(i);
      Serial.print(": UID[");
      for(int j = 0; j < UID_LEN; j++) {
        if(tagUIDs[i][j] < 0x10) Serial.print("0");
        Serial.print(tagUIDs[i][j], HEX);
        if(j < UID_LEN - 1) Serial.print(" ");
      }
      Serial.print("] → 노래 인덱스: ");
      Serial.print(songIndices[i]);
      Serial.print(" (파일: 00");
      Serial.print(songIndices[i] + SONG_START_INDEX + 2);
      Serial.println(".mp3)");
    }
  }
  Serial.print("총 ");
  Serial.print(tagCount);
  Serial.println("개 태그 등록됨");
  Serial.println("=======================");
}

void saveTagDataToEEPROM() {
  int addr = EEPROM_TAG_START_ADDR;

  // UID 저장
  for(int i = 0; i < NUM_TAGS; i++) {
    for(int j = 0; j < UID_LEN; j++) {
      EEPROM.update(addr++, tagUIDs[i][j]);
    }
  }

  // 노래 인덱스 저장 (addr은 현재 2 + 32*4 = 130)
  for(int i = 0; i < NUM_TAGS; i++) {
    EEPROM.update(addr++, songIndices[i] & 0xFF);
    EEPROM.update(addr++, (songIndices[i] >> 8) & 0xFF);
  }

  Serial.println("EEPROM에 태그 데이터 저장 완료");
}

void setup() {
  admin_SetUp();
  mf_Setup();
  df_Setup();

  // EEPROM에서 태그 데이터 복원
  loadTagDataFromEEPROM();

  // EEPROM에 저장된 마지막 재생 노래 읽어오기
  int lastSong = readLastSongFromEEPROM();
  Serial.print("마지막 재생 노래 인덱스: ");
  Serial.println(lastSong);

  // 전원 켜짐 음성 안내
  delay(500); // DFPlayer 초기화 대기
  mp3.playMp3FolderTrack(VOICE_POWER_ON + 1); // "전원이 켜졌습니다" (1-based)
  delay(2000); // 음성 재생 대기
}

void loop() {
  // 음량 조절 버튼 체크 (항상 활성화)
  static unsigned long lastVolumeChange = 0;
  unsigned long currentTime = millis();

  if(currentTime - lastVolumeChange > 200) {  // 디바운싱 200ms
    int volumeUpState = digitalRead(volumeUpPin);
    int volumeDownState = digitalRead(volumeDownPin);

    if(volumeUpState == LOW && currentVolume < 30) {  // 풀업이라 누르면 LOW
      currentVolume++;
      mp3.setVolume(currentVolume);
      saveVolumeToEEPROM(currentVolume);
      Serial.print("음량 증가: ");
      Serial.println(currentVolume);
      lastVolumeChange = currentTime;
    } else if(volumeDownState == LOW && currentVolume > 0) {  // 풀업이라 누르면 LOW
      currentVolume--;
      mp3.setVolume(currentVolume);
      saveVolumeToEEPROM(currentVolume);
      Serial.print("음량 감소: ");
      Serial.println(currentVolume);
      lastVolumeChange = currentTime;
    }
  }

  while(true) {
    int readState = digitalRead(readInput);
    int writeState = digitalRead(writeInput);

    if(readState == LOW && writeState == HIGH) {  // 풀업이라 반대로
      Serial.println(">> 1. 대기모드 버튼을 누르셨습니다.");
      mp3.stop();
      mp3.playMp3FolderTrack(VOICE_LISTEN_MODE + 1); // "읽기모드" (1-based)
      currentMod = LISTEN;
      delay(3000);
      break;
    } else if(writeState == LOW && readState == HIGH) {  // 풀업이라 반대로
      Serial.println(">> 2. 등록모드 버튼을 누르셨습니다.");
      mp3.playMp3FolderTrack(VOICE_REGISTER_MODE + 1); // "등록모드" (1-based)
      currentMod = REGISTER;
      delay(3000);
      break;
    } else if(readState == LOW && writeState == LOW) {  // 둘 다 누르면 둘 다 LOW
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
        songIndices[i] = 0; // 노래 인덱스도 초기화
      }

      // EEPROM에도 초기화된 데이터 저장
      saveTagDataToEEPROM();

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

    int slot = findTagSlot(rfid.uid.uidByte, tagUIDs, NUM_TAGS);

    if(slot != -1) {
      int songIndex = songIndices[slot];
      Serial.print("태그 발견! 슬롯: ");
      Serial.print(slot);
      Serial.print(", 노래 인덱스: ");
      Serial.println(songIndex);

      int trackNumber = songIndex + SONG_START_INDEX + 2;
      Serial.print("재생할 트랙: ");
      Serial.print(trackNumber);
      Serial.print(" (파일: ");
      if(trackNumber < 10) Serial.print("000");
      else if(trackNumber < 100) Serial.print("00");
      else if(trackNumber < 1000) Serial.print("0");
      Serial.print(trackNumber);
      Serial.println(".mp3)");
      delay(300);

      // 마지막 재생 노래 EEPROM에 저장
      saveLastSongToEEPROM(songIndex);

      // 노래 재생
      mp3.playMp3FolderTrack(trackNumber);
    } else {
      Serial.println("등록되지 않은 태그입니다.");
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
      int volumeUpState = digitalRead(volumeUpPin);
      int volumeDownState = digitalRead(volumeDownPin);

      // 볼륨 업 버튼으로 인덱스 증가
      if(volumeUpState == LOW) {
        Serial.print(F("index 증가: "));
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
        delay(300); // 디바운싱
      }

      // 볼륨 다운 버튼으로 인덱스 감소
      if(volumeDownState == LOW) {
        Serial.print(F("index 감소: "));
        index = (index - 1 + NUM_TAGS) % NUM_TAGS;
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
        delay(300); // 디바운싱
      }

      if(setRegister == LOW) {  // 풀업이라 누르면 LOW
        Serial.println(F(">> 등록할 태그키를 리더기에 붙여주세요."));
        Serial.print(F(">> target 음원 파일번호 : "));
        Serial.println(index);

        mp3.playMp3FolderTrack(VOICE_BRING_TAG + 1); // "태그키를 가져다대세요" (1-based)
        delay(2000); // 음성 재생 대기

        unsigned long registerStartTime = millis();

        while(true) {
          if(millis() - registerStartTime > 10000) {
            Serial.println("등록시간 초과, 등록모드 종료.");
            break;
          }
          if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
            continue;

          // 이미 등록된 태그인지 확인
          int existingSlot = findTagSlot(rfid.uid.uidByte, tagUIDs, NUM_TAGS);

          if(existingSlot != -1) {
            // 이미 등록된 태그 - 노래 인덱스만 업데이트
            Serial.print("기존 태그 발견 (슬롯 ");
            Serial.print(existingSlot);
            Serial.print(") - 노래 인덱스 업데이트: ");
            Serial.print(songIndices[existingSlot]);
            Serial.print(" → ");
            Serial.println(index);

            songIndices[existingSlot] = index;
          } else {
            // 새로운 태그 - 빈 슬롯에 저장
            int emptySlot = findEmptySlot();
            if(emptySlot == -1) {
              Serial.println("태그 슬롯이 가득 찼습니다! (최대 32개)");
              return;
            }

            Serial.print("새 태그 등록 → 슬롯 ");
            Serial.print(emptySlot);
            Serial.print(", UID: ");
            for(int i = 0; i < UID_LEN; i++) {
              tagUIDs[emptySlot][i] = rfid.uid.uidByte[i];
              if(rfid.uid.uidByte[i] < 0x10) Serial.print("0");
              Serial.print(rfid.uid.uidByte[i], HEX);
              Serial.print(" ");
            }
            Serial.print(", 노래 인덱스: ");
            Serial.println(index);

            songIndices[emptySlot] = index;
          }

          // 변경된 데이터 EEPROM에 저장
          saveTagDataToEEPROM();

          // 등록 완료 음성 안내
          mp3.playMp3FolderTrack(VOICE_REGISTER_DONE + 1); // "등록완료되었습니다" (1-based)
          delay(2000); // 음성 재생 대기

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
        delay(1000); // 타임아웃 후 1초 대기
        break;
      } else if(indexInc == LOW) {  // 읽기버튼: 현재 인덱스를 음성으로 알려줌
        Serial.print(F("현재 index 음성 재생: "));
        Serial.println(index);

        // 인덱스 번호를 음성으로 재생
        playIndexVoice(index);

        delay(300); // 디바운싱
      }
      temp = indexInc;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

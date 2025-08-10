#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // 6_1에서 시리얼 모니터를 통하여 0x27이 시작주소인걸 확인하였다.

void setup() {
  lcd.init(); // I2C LCD의 초기화
  lcd.backlight(); // I2C LCD의 백라이트 켜기
  

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Welconme to");
  lcd.setCursor(1, 1);
  lcd.print("ARDUINO world !");

}

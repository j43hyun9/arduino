#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte char__00[8] = {0x00, 0x0E, 0x1B, 0x11, 0x11, 0x1B, 0x0E, 0x00};
byte char__01[8] = {0x08, 0x08, 0x08, 0x0E, 0x08, 0x08, 0x08, 0x08};
byte char__02[8] = {0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00};
byte char__12[8] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00};
byte char__05[8] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
byte char__06[8] = {0x00, 0x10, 0x10, 0x10, 0x1E, 0x00, 0x00, 0x00};
byte char__16[8] = {0x04, 0x04, 0x04, 0x04, 0x1F, 0x00, 0x00, 0x00};
byte heart[8] = {0x00, 0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00};
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, char__00);
  lcd.createChar(0, char__01);
  lcd.createChar(0, char__02);
  lcd.createChar(0, char__05);
  lcd.createChar(0, char__06);
  lcd.createChar(0, char__12);
  lcd.createChar(0, char__16);
  lcd.createChar(0, heart);
  lcd.clear();

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.write(byte(7));
  lcd.setCursor(2, 0);
  lcd.write(byte(0));
  lcd.setCursor(3, 0);
  lcd.write(byte(1));
  lcd.setCursor(4,0);
  lcd.write(byte(2));
  lcd.setCursor(4,1);
  lcd.write(byte(5));
  lcd.setCursor(6,0);
  lcd.write(byte(0));
  lcd.setCursor(7,0);
  lcd.write(byte(3));
  lcd.setCursor(8,0);
  lcd.write(byte(4));
  lcd.setCursor(8, 1);
  lcd.write(byte(6));
}

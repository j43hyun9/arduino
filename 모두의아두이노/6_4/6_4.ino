#include <LiquidCrystal_I2C.h>

char array1[]="Hello, world!        ";
char array2[]="Nice to meet you.    ";
int delayTime = 500;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();

}

void loop() {
  lcd.setCursor(15,0);
  for (int posCnt1 = 0; posCnt1 < 26; posCnt1++)
  {
    lcd.scrollDisplayLeft();
    lcd.print(array1[posCnt1]);
    delay(delayTime);
  }

  lcd.clear();
  lcd.setCursor(15, 1);
  for (int posCnt2 = 0; posCnt2 < 26; posCnt2++)
  {
    lcd.scrollDisplayLeft();
    lcd.print(array2[posCnt2]);
    delay(delayTime);
  }
  lcd.clear();

}

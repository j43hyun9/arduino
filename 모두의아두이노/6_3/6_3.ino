#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();

}

void loop() {
  lcd.print("Hello, arduino!");
  lcd.noDisplay();
  delay(1000);
  lcd.display();
  delay(1000);
  lcd.clear();

}

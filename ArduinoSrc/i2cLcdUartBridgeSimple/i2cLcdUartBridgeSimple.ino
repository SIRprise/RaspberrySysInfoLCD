#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

const byte lcdAddress = 0x27;
byte lcdCols = 20;
byte lcdRows = 4;

LiquidCrystal_PCF8574 lcd(lcdAddress);

void setup() {
  int error;
  
  Serial.begin(115200);
  while (! Serial);

  Wire.begin();
  Wire.beginTransmission(lcdAddress);
  error = Wire.endTransmission();

  if (error == 0)
  {
    lcd.begin(lcdCols, lcdRows);
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
  }
}

void loop() {

}

void serialEvent() {  
  static byte rowIdx = 0;
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    switch(inChar)
    {
      case '\r':
      case '\n':
        if((rowIdx+1) < lcdRows)
        {
          rowIdx++;
          lcd.setCursor(0,rowIdx);
        }
        else
        {
          rowIdx = 0;
          lcd.home();
          lcd.clear();  
        }
        break;  
        
      case 0x255:
        lcd.home();
        lcd.clear();
        break;
        
      default:
        lcd.print(inChar);
        break;
    }    
  }
}

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define ACK 0
#define NACK 1
#define SOF 255
#define EOF 254
#define ENC 253

enum ECMD { 
  CMD_DUMMY = 5,
  CMD_SEARCH,
  CMD_PARAMS,
  CMD_INIT,
  CMD_BACKLIGHT,
  CMD_HOME,
  CMD_CLEAR,
  CMD_NO_DISP,
  CMD_DISP,
  CMD_SET_CURSOR,
  CMD_SCROLL_LEFT,
  CMD_SCROLL_RIGHT,
  CMD_PRINT
  };

const byte lcdAddress = 0x27;
byte lcdCols = 20;
byte lcdRows = 4;

LiquidCrystal_PCF8574 lcd(lcdAddress);

char frameData[255];
byte frameCount = 0;

void setup() {
  Serial.begin(115200);
  while (! Serial);
}

void loop() {
  // everything handled in cmd handler of serial
}

void setLcdParams(byte cols, byte rows)
{
  lcdCols = cols;
  lcdRows = rows;
}

bool lcdInit() {
  int error;

  Wire.begin();
  Wire.beginTransmission(lcdAddress);
  error = Wire.endTransmission();

  if (error == 0)
  {
    lcd.begin(lcdCols, lcdRows);
    return true;
  }
  else
  {
    return false;
  }
}

//returns address of found i2c device or 0 if nothing found
byte lcdSearchAddress() {
  byte error, address;

  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      return address;
    }
  }
  return 0;
}

void sendResult(byte res)
{
  Serial.write(SOF);
  Serial.write(res);
  Serial.write(EOF);
}

void sendResultParam(byte res, byte param)
{
  Serial.write(SOF);
  Serial.write(res);
  Serial.write(param);
  Serial.write(EOF);
}

void frameHandler()
{
  //decode and enter commandHandler
  if(frameCount>1)
  {
    ECMD c = frameData[0];
    commandHandler(c,&(frameData[1]),frameCount-1);
  }
  
  if(frameCount>0)
  {
    ECMD c = frameData[0];
    commandHandler(c,NULL,0);
  }
  
  frameCount = 0;
}

void commandHandler(ECMD cmd, byte param[], byte paramCount)
{
  switch(cmd)
  {
    case CMD_DUMMY:
      //could be used as ACK or connection sense later
      sendResult(ACK);
      break;

    case CMD_SEARCH:
    {
      byte res = lcdSearchAddress();
      if(res != 0)
        sendResultParam(ACK,res);
      else
        sendResult(NACK);
      break;
    }

    case CMD_PARAMS:
      if(paramCount<2)
        sendResult(NACK);
      else
      {
        setLcdParams(param[0],param[1]);
        sendResult(ACK);
      }
      break;

    case CMD_INIT:
      if(lcdInit())
        sendResult(ACK);
      else
        sendResult(NACK);
      break;

    case CMD_BACKLIGHT:
      if(paramCount<1)
        sendResult(NACK);
      else
      {
        lcd.setBacklight(param[0]);
        sendResult(ACK);
      }
      break;

    case CMD_HOME:
      lcd.home();
      sendResult(ACK);
      break;

    case CMD_CLEAR:
      lcd.clear();
      sendResult(ACK);
      break;

    case CMD_DISP:
      lcd.noDisplay();
      sendResult(ACK);
      break;

    case CMD_NO_DISP:
      lcd.display();
      sendResult(ACK);
      break;

    case CMD_SET_CURSOR:
      if(paramCount<2)
        sendResult(NACK);
      else
      {
        lcd.setCursor(param[0],param[1]);
        sendResult(ACK);
      }
      break;

    case CMD_SCROLL_LEFT:
      lcd.scrollDisplayLeft();
      sendResult(ACK);
      break;

    case CMD_SCROLL_RIGHT:
      lcd.scrollDisplayRight();
      sendResult(ACK);
      break;
      
    case CMD_PRINT:
      if(paramCount<1)
        sendResult(NACK);
      else
      {
        lcd.print((char*)param);
        sendResult(ACK);
      } 
      break;
  }
}

void serialEvent() {
  static bool frameStarted = false;
  static bool encOn = false;
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if(inChar == SOF)
    {
      frameStarted = true;
      encOn = false;
      continue;
    }
    
    if(inChar == EOF)
    {
      frameStarted = false;
      //start handler
      frameHandler();
    }

    if(inChar == ENC)
    {
      encOn = true;
      continue;
    }

    if((encOn) && (inChar == 0x20))
    {
      frameData[frameCount++] = ENC;
      encOn = false;
      continue;
    }

    if((encOn) && (inChar == 0x21))
    {
      frameData[frameCount++] = EOF;
      encOn = false;
      continue;
    }

    if((encOn) && (inChar == 0x22))
    {
      frameData[frameCount++] = SOF;
      encOn = false;
      continue;
    }
    
    frameData[frameCount++] = inChar;
  }
}



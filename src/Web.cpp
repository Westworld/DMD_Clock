#include "Web.h"
#include "Video.h"
#include "EEPROM.h"
#include "digits.h"

extern u_int8_t Flash_version;
extern bool twelveHourFormat;  
extern bool displaySeconds ;
extern u_int8_t displayTime ; 
extern u_int16_t frameColor; 

extern Digits  * clockdigits;

extern int16_t timeCounter;

#define webdebug 1

#define EEPROM_SIZE 9

void Flash_Read() {
  // check if our structure
  EEPROM.begin(EEPROM_SIZE);
  int8_t check = EEPROM.read(0);
  if (check != 0x4D) 
    Flash_Write(0x4D);
  else {
    int8_t version = EEPROM.read(1);

    switch (version) {
      case 1:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        clockdigits->fontcolor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);        
        break;
      default:
        Flash_Write(0x4D);
    }
  }
  EEPROM.end();
}

void Flash_Write(int8_t what) {
  EEPROM.begin(EEPROM_SIZE);
  Serial.print("what: ");
  Serial.println(what); 
 switch(what) {
   case 0x4D:  // all
    DebugString("Flash Init");
    delay(2000);
    EEPROM.write(0, 0x4D);
    EEPROM.write(1, Flash_version);
    EEPROM.write(2, displayTime);
    EEPROM.write(3, twelveHourFormat);
    EEPROM.write(4, displaySeconds);
    EEPROM.writeShort(5, clockdigits->fontcolor);
    EEPROM.writeShort(7, frameColor);
    break;

   case 2:
    EEPROM.write(2, displayTime); break;
   case 3:
    EEPROM.write(3, twelveHourFormat); break;       
   case 4:
    EEPROM.write(4, displaySeconds); break;
   case 5:
    EEPROM.writeShort(5, clockdigits->fontcolor); break;   
   case 7:
    EEPROM.writeShort(7, frameColor); break;
   } 

   EEPROM.commit();
   EEPROM.end();
}

void color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b){
  r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  b = (((color & 0x1F) * 527) + 23) >> 6;
}

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

//uint16_t color565(uint32_t rgb32) {
//  return (rgb32>>8&0xf800)|(rgb32>>5&0x07e0)|(rgb32>>3&0x001f);
// }

uint16_t color565( uint32_t rgb)
{
  uint16_t R = (rgb >> 16) & 0xFF;
  uint16_t G = (rgb >>  8) & 0xFF;
  uint16_t B = (rgb      ) & 0xFF;

#ifdef UseDMD
  uint16_t ret  = (R & 0xF8) << 8;  // 5 bits
           ret |= (B & 0xFC) << 3;  // 6 bits
           ret |= (G & 0xF8) >> 3;  // 5 bits     
  return( ret);
#else
  uint16_t ret  = (R & 0xF8) << 8;  // 5 bits
           ret |= (G & 0xFC) << 3;  // 6 bits
           ret |= (B & 0xF8) >> 3;  // 5 bits     
  return( ret);
#endif
}

String ConvertColor565to888hex(uint16_t color) {
  uint8_t r, g, b;
  color565to888(color, r, g, b);
  char hexout[8];
  sprintf(hexout,"#%02x%02x%02x",r,g,b);
  String result = hexout;
  return result;
}  


void Web_timedisplayCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Time Display: "+sender->value);
  #endif

  displayTime = sender->value.toInt();
  timeCounter = 0;
  Flash_Write(2);
}

void Web_timecolorCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Time Color: "+sender->value);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  clockdigits->fontcolor= color565(color);
  timeCounter = 0;
  Flash_Write(5);
}

void Web_timeframeCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Frame Color: "+sender->value);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  frameColor= color565(color);
  timeCounter = 0;
  Flash_Write(7);
}

void Web_Init() {
  ESPUI.number("Time display in seconds:", &Web_timedisplayCall, ControlColor::Alizarin, displayTime);
  String colorstring = ConvertColor565to888hex(clockdigits->fontcolor);
  uint16_t text_colour = ESPUI.text("Time display color:", &Web_timecolorCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  colorstring = ConvertColor565to888hex(frameColor);
  text_colour = ESPUI.text("Time frame color:", &Web_timeframeCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  ESPUI.begin("DMD Clock");
  
}
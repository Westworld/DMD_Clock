#include "settings.h"
#include "EEPROM.h"

#define EEPROM_SIZE 16

Settings::Settings(){
    Flash_Read();     
    }

int8_t Settings::getFontNumber(void){
    return fontnumber;
};    

bool Settings::getDisplaySeconds(void){
    return displaySeconds;
}; 

bool Settings::getTwelveHourFormat(void){
    return twelveHourFormat;
}; 

uint16_t Settings::getFrameColor(void){
    return frameColor;
}; 

uint16_t Settings::getFontColor(void){
    return fontColor;
}; 

uint16_t Settings::getFontSparkleColor(void){
    return fontSparkleColor;
}; 

uint8_t Settings::getDisplayTime(void) {
    return displayTime;
}

uint8_t Settings::getTimeZoneArea(void) {
    return timezonearea;
}

uint8_t Settings::getTimeZoneID(void) {
    return timezoneid;
}

bool Settings::getClockUpDown(void) {
    return ClockUpDown;
}

bool Settings::getClockSparkle(void) {
    return ClockSparkle;
}

void Settings::setDisplayTime(uint8_t newTime) {
    if (newTime != displayTime) {
        displayTime = newTime;
        Flash_Write(2);
    }
}

void Settings::setFontColor(uint16_t color) {
    if (color != fontColor) {
        fontColor = color;
        Flash_Write(5);
        doRefresh();
    }
}

void Settings::setFontSparkleColor(uint16_t color) {
    if (color != fontSparkleColor) {
        fontSparkleColor = color;
        Flash_Write(14);
        doRefresh();
    }
}
void Settings::setFrameColor(uint16_t color) {
    if (color != frameColor) {
        frameColor = color;
        Flash_Write(7);
        doRefresh();
    }
}

void Settings::setFontNumber(uint8_t font, bool noFlash) {
    if (font != fontnumber) {
        fontnumber = font;
        if (!noFlash)
            Flash_Write(9);
        doRefresh();
    }
}

void Settings::setDisplaySeconds(bool display) {
    if (display != displaySeconds) {
        displaySeconds = display;
        Flash_Write(4);
        doRefresh();
    }
}

void Settings::setTwelveHourFormat(bool display) {
    if (display != twelveHourFormat) {
        twelveHourFormat = display;
        Flash_Write(3);
        doRefresh();
    }
}

void Settings::setTimeZone(uint8_t ID) {
    timezoneid = ID;
    Flash_Write(10);
    doRefresh();
}
void Settings::setTimeZone(uint8_t Area, uint8_t ID) {
    timezonearea = Area;
    timezoneid = ID;
    Flash_Write(10);
    doRefresh();
}

void Settings::setClockUpDown(bool display) {
    if (display != ClockUpDown) {
        ClockUpDown = display;
        Flash_Write(12);
        doRefresh();
    }
}

void Settings::setClockSparkle(bool display) {
    if (display != ClockSparkle) {
        ClockSparkle = display;
        Flash_Write(13);
        doRefresh();
    }
}

void Settings::doRefresh(void) {
    DisplayRefresh=true;
}
bool Settings::needRefresh(void) {
    if(DisplayRefresh)
    {
        DisplayRefresh=false;
        return true;
    }
    else   
        return false;
}


void Settings::Flash_Read() {
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
        fontColor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);     
        fontnumber = 0;  
        Flash_Write(0x4D); 
        break;

      case 2:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        fontColor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);
        fontnumber = EEPROM.read(9);             
        Flash_Write(0x4D); 
       break;     

      case 3:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        fontColor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);
        fontnumber = EEPROM.read(9);   
        timezonearea = EEPROM.read(10);
        timezoneid = EEPROM.read(11);
        Flash_Write(0x4D); 
        break;   

      case 4:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        fontColor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);
        fontnumber = EEPROM.read(9);   
        timezonearea = EEPROM.read(10);
        timezoneid = EEPROM.read(11);
        ClockUpDown = EEPROM.read(12);
        ClockSparkle = EEPROM.read(13);
        fontSparkleColor = EEPROM.readShort(14);
        break;   

      default:
        Flash_Write(0x4D);
    }
  }
  EEPROM.end();
  #ifdef webdebug  
  Serial.println("FlashRead TZ="+String(timezonearea)+"/"+String(timezoneid)+" displayTime="+String(displayTime)+" font="+String(fontnumber)+" fontcolor="+String(fontColor));
  #endif
  if (fontColor == 0)
    fontColor = 0xFFFF;  // White
}

void Settings::Flash_Write(int8_t what) {
  EEPROM.begin(EEPROM_SIZE);
    #ifdef webdebug 
        Serial.print("flash write: ");
        Serial.println(what); 
    #endif
 switch(what) {
   case 0x4D:  // all
    #ifdef webdebug 
        Serial.println("Flash Init");
    #endif
    EEPROM.write(0, 0x4D);
    EEPROM.write(1, 4);  // version
    EEPROM.write(2, displayTime);
    EEPROM.write(3, twelveHourFormat);
    EEPROM.write(4, displaySeconds);
    EEPROM.writeShort(5, fontColor);
    EEPROM.writeShort(7, frameColor);
    EEPROM.write(9, fontnumber);  
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);  
    EEPROM.write(12, ClockUpDown);  
    EEPROM.write(13, ClockSparkle); 
    EEPROM.writeShort(14, fontSparkleColor); 
    break;

   case 2:
    EEPROM.write(2, displayTime); 
    break;
   case 3:
    EEPROM.write(3, twelveHourFormat); 
    break;       
   case 4:
    EEPROM.write(4, displaySeconds); 
    break;
   case 5:
    EEPROM.writeShort(5, fontColor); 
    break;   
   case 7:
    EEPROM.writeShort(7, frameColor); 
    break;
   case 9:
    EEPROM.write(9, fontnumber);
    break;  
   case 10:
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);    
    break;
   case 12:
    EEPROM.write(12, ClockUpDown); 
    break;
   case 13:
    EEPROM.write(13, ClockSparkle); 
    break;     
   case 14:
    EEPROM.writeShort(14, fontSparkleColor);

    // #### when enhance, increase EEPROM size!!!
} 

   EEPROM.commit();
   EEPROM.end();
}
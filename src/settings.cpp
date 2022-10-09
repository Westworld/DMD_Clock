#include "settings.h"
#include "EEPROM.h"

#define EEPROM_SIZE 12

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

u_int8_t Settings::getDisplayTime(void) {
    return displayTime;
}

u_int8_t Settings::getTimeZoneArea(void) {
    return timezonearea;
}

u_int8_t Settings::getTimeZoneID(void) {
    return timezoneid;
}

void Settings::setDisplayTime(u_int8_t newTime) {
    if (newTime != displayTime) {
        displayTime = newTime;
        Flash_Write(2);
    }
}

void Settings::setFontColor(u_int16_t color) {
    if (color != fontColor) {
        fontColor = color;
        Flash_Write(5);
        doRefresh();
    }
}

void Settings::setFrameColor(u_int16_t color) {
    if (color != frameColor) {
        frameColor = color;
        Flash_Write(7);
        doRefresh();
    }
}

void Settings::setFontNumber(u_int8_t font) {
    if (font != fontnumber) {
        fontnumber = font;
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

void Settings::setTimeZone(u_int8_t ID) {
    timezoneid = ID;
    Flash_Write(10);
    doRefresh();
}
void Settings::setTimeZone(u_int8_t Area, u_int8_t ID) {
    timezonearea = Area;
    timezoneid = ID;
    Flash_Write(10);
    doRefresh();
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
    EEPROM.write(1, 3);  // version
    EEPROM.write(2, displayTime);
    EEPROM.write(3, twelveHourFormat);
    EEPROM.write(4, displaySeconds);
    EEPROM.writeShort(5, fontColor);
    EEPROM.writeShort(7, frameColor);
    EEPROM.write(9, fontnumber);  
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);  
    break;

   case 2:
    EEPROM.write(2, displayTime); break;
   case 3:
    EEPROM.write(3, twelveHourFormat); break;       
   case 4:
    EEPROM.write(4, displaySeconds); break;
   case 5:
    EEPROM.writeShort(5, fontColor); break;   
   case 7:
    EEPROM.writeShort(7, frameColor); break;
   case 9:
    EEPROM.write(9, fontnumber);
    break;  
   case 10:
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);    
    break;
} 

   EEPROM.commit();
   EEPROM.end();
}
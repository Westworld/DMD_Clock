#include "settings.h"
#include "EEPROM.h"
#include "ArduinoJson.h"
#include "SD.h"

#define EEPROM_SIZE 17

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

bool Settings::getClockBlend(void) {
    return ClockBlend;
}

String Settings::getSSIDPass(short was) {
    switch (was) {
        case 1: return SSID1; 
        case 2: return PASS1; 
        case 3: return SSID2; 
        case 4: return PASS2; 
        case 5: return SSID3; 
        case 6: return PASS3; 
        
    }
    return "";

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

void Settings::setClockBlend(bool display) {
    if (display != ClockBlend) {
        ClockBlend = display;
        Flash_Write(16);
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
  // check if settings on SD Card
  if (SD.exists("/config.json"))
    { 
        JsonDocument doc;

        File card = SD.open("/config.json",FILE_READ);
        deserializeJson(doc, card);
        card.close();

        if (!doc["displayTime"].isNull())
            displayTime = doc["displayTime"].as<uint8_t>();

        if (!doc["twelveHourFormat"].isNull())
            twelveHourFormat = doc["twelveHourFormat"].as<bool>();
        if (!doc["displaySeconds"].isNull())
            displaySeconds = doc["displaySeconds"].as<bool>();

        if (!doc["fontColor"].isNull())
            fontColor = doc["fontColor"].as<uint16_t>();
        if (!doc["frameColor"].isNull())
            frameColor = doc["frameColor"].as<uint16_t>();
        if (!doc["fontnumber"].isNull())
            fontnumber = doc["fontnumber"].as<uint8_t>();
        if (!doc["timezonearea"].isNull())
            timezonearea = doc["timezonearea"].as<uint8_t>();
        if (!doc["timezoneid"].isNull())
            timezoneid = doc["timezoneid"].as<uint8_t>();

        if (!doc["ClockUpDown"].isNull())
            ClockUpDown = doc["ClockUpDown"].as<bool>();
        if (!doc["ClockSparkle"].isNull())
            ClockSparkle = doc["ClockSparkle"].as<bool>();
        if (!doc["fontSparkleColor"].isNull())
            fontSparkleColor = doc["fontSparkleColor"].as<bool>();
        if (!doc["ClockBlend"].isNull())
            ClockBlend = doc["ClockBlend"].as<bool>();            

        if (!doc["Wifi"][0]["SSID"].isNull())
            SSID1 = doc["Wifi"][0]["SSID"].as<String>();  
        if (!doc["Wifi"][0]["PASS"].isNull())
            PASS1 = doc["Wifi"][0]["PASS"].as<String>(); 
        if (!doc["Wifi"][1]["SSID"].isNull())
            SSID2 = doc["Wifi"][1]["SSID"].as<String>();  
        if (!doc["Wifi"][1]["PASS"].isNull())
            PASS2 = doc["Wifi"][1]["PASS"].as<String>(); 
        if (!doc["Wifi"][2]["SSID"].isNull())
            SSID3 = doc["Wifi"][2]["SSID"].as<String>();  
        if (!doc["Wifi"][2]["PASS"].isNull())
            PASS3 = doc["Wifi"][2]["PASS"].as<String>(); 

        if (fontColor == 0)
            fontColor = 0xFFFF;  // White
    }
else {
  // in previous versions use EEPROM. Removed...

}

}

void Settings::Flash_Write(int8_t what) {


    JsonDocument doc;

    doc["Wifi"][0]["SSID"] = WIFI_SSID;
    doc["Wifi"][0]["Pass"] = WIFI_PASS;
    doc["Wifi"][1]["SSID"] = WIFI_SSID3;
    doc["Wifi"][1]["Pass"] = WIFI_PASS3;

    doc["version"] = 5;
    doc["displayTime"] = displayTime;
    doc["twelveHourFormat"] = twelveHourFormat;
    doc["displaySeconds"] = displaySeconds;
    doc["fontColor"] = fontColor;
    doc["frameColor"] = frameColor;
    doc["fontnumber"] = fontnumber;
    doc["timezonearea"] = timezonearea;
    doc["timezoneid"] = timezoneid;
    doc["ClockUpDown"] = ClockUpDown;
    doc["ClockSparkle"] = ClockSparkle;
    doc["fontSparkleColor"] = fontSparkleColor;
    doc["ClockBlend"] = ClockBlend;

   #ifdef webdebug 
        serializeJson(doc, Serial);
    #endif

    File card;
    card = SD.open("/config.json",FILE_WRITE);
    serializeJson(doc, card);
    card.close();

}

#ifndef Settings_h
#define Settings_h 

#include "Arduino.h"

class Settings
{
  public:  
    Settings(void);
    int8_t getFontNumber(void);
    bool getDisplaySeconds(void);
    bool getTwelveHourFormat(void);
    uint16_t getFrameColor(void);
    uint16_t getFontColor(void);
    u_int8_t getDisplayTime(void);
    u_int8_t getTimeZoneArea(void);
    u_int8_t getTimeZoneID(void);

    bool needRefresh(void);
    void doRefresh(void);

    void setDisplayTime(u_int8_t newTime);
    void setFontColor(u_int16_t color);
    void setFrameColor(u_int16_t color);
    void setFontNumber(u_int8_t font);
    void setDisplaySeconds(bool display);
    void setTwelveHourFormat(bool display);
    void setTimeZone(u_int8_t ID);
    void setTimeZone(u_int8_t Area, u_int8_t ID);
     
  protected:
    u_int8_t displayTime=20;
    bool twelveHourFormat = false;  
    bool displaySeconds = false;
    u_int16_t frameColor = 0xF800; // TFT_RED
    uint16_t fontColor=0xFFFF;  // TFT_WHITE
    int8_t fontnumber=0; 
    u_int8_t timezonearea=8;  // Europe
    u_int8_t timezoneid=0; // first entry
    bool DisplayRefresh = false;

    void Flash_Read(void);
    void Flash_Write(int8_t what);
};

#endif
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
    uint16_t getFontSparkleColor(void);
    u_int8_t getDisplayTime(void);
    u_int8_t getTimeZoneArea(void);
    u_int8_t getTimeZoneID(void);
    bool getClockUpDown(void);
    bool getClockSparkle(void);
    bool getClockBlend(void);  // draw clock over last video or blend over

    bool needRefresh(void);
    void doRefresh(void);

    void setDisplayTime(uint8_t newTime);
    void setFontColor(uint16_t color);
    void setFontSparkleColor(uint16_t color);
    void setFrameColor(uint16_t color);
    void setFontNumber(uint8_t font, bool noFlash=false);
    void setDisplaySeconds(bool display);
    void setTwelveHourFormat(bool display);
    void setTimeZone(uint8_t ID);
    void setTimeZone(uint8_t Area, u_int8_t ID);
    void setClockUpDown(bool display);
    void setClockSparkle(bool display);
    void setClockBlend(bool display);
     
  protected:
    uint8_t displayTime=20;
    bool twelveHourFormat = false;  
    bool displaySeconds = false;
    u_int16_t frameColor = 0xF800; // TFT_RED
    uint16_t fontColor=0xFFFF;  // TFT_WHITE
    uint16_t fontSparkleColor=0x0000; // black
    int8_t fontnumber=0; 
    uint8_t timezonearea=8;  // Europe
    uint8_t timezoneid=0; // first entry
    bool ClockUpDown = false;
    bool ClockSparkle = false;
    bool DisplayRefresh = false;
    bool ClockBlend = true;

    void Flash_Read(void);
    void Flash_Write(int8_t what);
};

#endif
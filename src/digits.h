#ifndef Digits_h
#define Digits_h 

#include "display.h"
#include "settings.h"

#define BLACK 0x0000

class Digits
{
  public:     
    Display *display;
    Settings *settings;

    Digits( Display * thedisplay, Settings *thesettings);
    void SetFont(String namen);
    void SetFontNumber(int8_t number);
    void CheckFont();
    int8_t DrawChar(char thechar, int8_t x, int8_t y, int16_t color);
    int8_t DrawDigit(int8_t digit, int8_t x, int8_t y, int16_t color) ;
    void DrawString(String text, int8_t x, int8_t y, int16_t color);
    int8_t CalcDigitWidth(int8_t digit);
    int16_t CalcTimeWidth( int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int8_t distance);
    void DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int16_t& timeCounter) ;

    protected:
        char * fontbuffer = NULL;
        int8_t charwidths[11];
        int16_t charoffset[11];
        String fontname;
        int8_t height;
        int8_t count;
        int16_t fontsize; // length of fontbuffer;
        int8_t lastcolon, lastseccolon;
        int8_t fontnumber; // local copy
        int8_t upDownCounter=0;
};


#endif
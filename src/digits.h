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
    int8_t DrawChar(char thechar, int8_t x, int8_t y, uint16_t color);
    int8_t DrawDigit(int8_t digit, int8_t x, int8_t y, uint16_t color) ;
    int8_t DrawDigitCheck(int8_t digit, int8_t x, int8_t y, int8_t type );
    void DrawString(String text, int8_t x, int8_t y, uint16_t color);
    int8_t CalcDigitWidth(int8_t digit);
    int16_t CalcTimeWidth( int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int8_t distance);
    void DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int16_t& timeCounter) ;
    int8_t DrawTime (int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int8_t draw1, int8_t draw2, int8_t draw3, int8_t draw4, int8_t draw5, int8_t draw6, int8_t draw7, int8_t draw8);

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
        int16_t last_hour = -1;
        int16_t last_min  = -1;
        int16_t last_sec  = -1;  
        int16_t start_x;
};


#endif
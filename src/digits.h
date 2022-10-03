#ifdef UseDMD
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#else
#include <Arduino_GFX_Library.h>

#define BLACK 0x0000
#endif

#ifndef Digits_h
#define Digits_h 

class Digits
{
  public:  
    uint16_t fontcolor=0xFFFF;
    int8_t fontnumber;    

#ifdef UseDMD
    MatrixPanel_I2S_DMA *display;
    Digits( MatrixPanel_I2S_DMA *thedisplay);
#else
    Arduino_GFX *display;
    Digits(Arduino_GFX *thedisplay);
#endif    
    void SetFont(String namen);
    void SetFontNumber(int8_t number);
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
};


#endif
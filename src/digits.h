#ifdef UseDMD
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#else
#include <Arduino_GFX_Library.h>
#endif

#ifndef Digits_h
#define Digits_h 

class Digits
{
  public:  
    uint16_t fontcolor;

#ifdef UseDMD
    MatrixPanel_I2S_DMA *display;
    Digits( MatrixPanel_I2S_DMA *display);
#else
    Arduino_GFX *display;
    Digits(Arduino_GFX *display);
#endif    
    void SetFont(String namen);
    int8_t DrawChar(char thechar, int8_t x, int8_t y, int16_t color);
    void DrawString(String text, int8_t x, int8_t y, int16_t color);
    void DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec) ;

    protected:
        char * fontbuffer = NULL;
        char charwidths[11];
        int16_t charoffset[11];
        String fontname;
        int8_t height;
        int8_t count;
        int16_t fontsize; // length of fontbuffer;

};


#endif
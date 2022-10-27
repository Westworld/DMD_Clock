#include "display.h"

#ifdef UseDMD
  HUB75_I2S_CFG::i2s_pins _pins={PIN_R1, PIN_G1, PIN_B1, PIN_R2, PIN_G2, PIN_B2, PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_LE, PIN_OE, PIN_CLK};
  HUB75_I2S_CFG mxconfig(
          PANEL_WIDTH,   // width
          PANEL_HEIGHT,   // height
           PANELS_NUMBER,   // chain length
         _pins//,   // pin mapping
         //HUB75_I2S_CFG::FM6126A      // driver chip
);
#endif

    Display::Display() {
        Init();
    };

    void Display::Init() {
        #ifdef UseDMD
            display = new MatrixPanel_I2S_DMA(mxconfig);
            display->begin();
            display->setRotation(0);       
        #else
            tft = TFT_eSPI();  
            display = &tft;
            display->begin();
            display->setRotation(2); 
        #endif 
    }

void Display::StartScreen() {
  display->setTextColor(TFT_WHITE);
  display->fillScreen(TFT_BLACK);
  DrawRect(0, 0, 128, 32, TFT_RED);
  DrawString("DMD Clock", 0);    
};

void Display::DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  #ifdef UseDMD
    display->drawRect(x, y, w, h, color);
  #else
    display->drawRect(x*2, y*2, w*2, h*2, color);
    display->drawRect(x*2+1, y*2+1, w*2-2, h*2-2, color);
  #endif    
}

void Display::FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  #ifdef UseDMD
    display->fillRect(x, y, w, h, color);
  #else
    display->fillRect(x*2, y*2, w*2, h*2, color);
  #endif
}

void Display::DrawPixel(int16_t x, int16_t y, uint16_t color) {
    #ifdef UseDMD
        display->drawPixel(x, y, color); 
    #else
        display->drawRect(x*2, y*2, 2, 2, color);
    #endif      
};


void Display::DrawPixel(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b ) {
    #ifdef UseDMD
      display->drawPixelRGB888(x, y, r, g, b); 
    #else
        uint16_t color = color565( r, g,b);
        display->drawRect(x*2, y*2, 2, 2, color);
    #endif      
};

void Display::DrawString(String thetext, int8_t line = 0) {
    display->setTextColor(TFT_WHITE, 0x0000);
    display->setCursor(5, 5+(line*12));   
    display->print(thetext);
}

void Display::Clear(void) {
    display->fillScreen(TFT_BLACK);
}

void Display::color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b){
  r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  b = (((color & 0x1F) * 527) + 23) >> 6;
}

uint16_t Display::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t Display::color565( uint32_t rgb)
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

String Display::ConvertColor565to888hex(uint16_t color) {
  uint8_t r, g, b;
  color565to888(color, r, g, b);
  char hexout[8];
  sprintf(hexout,"#%02x%02x%02x",r,g,b);
  String result = hexout;
  return result;
}  

int16_t Display::height() {
    return display->height();
}
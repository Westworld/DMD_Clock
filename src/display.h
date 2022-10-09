#ifdef UseDMD
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#else
#include <TFT_eSPI.h>
#endif

#ifndef Display_h
#define Display_h 


#ifdef UseDMD
#define PANEL_WIDTH 64 // width: number of LEDs for 1 pannel
#define PANEL_HEIGHT 32 // height: number of LEDs
#define PANELS_NUMBER 2   // Number of horizontally chained panels 
#define PANE_WIDTH (PANEL_WIDTH*PANELS_NUMBER)
#define PANE_WIDTH_PLANE (PANE_WIDTH>>3)
#define PANE_HEIGHT PANEL_HEIGHT

    #define PIN_R1  25
    #define PIN_G1  26
    #define PIN_B1  27
    #define PIN_R2  18  
    #define PIN_G2  21 
    #define PIN_B2  33 

    #define PIN_A   23
    #define PIN_B   19
    #define PIN_C   5
    #define PIN_D   22
    #define PIN_E   -1 
              
    #define PIN_LE 4 // LAT 
    #define PIN_OE  32 
    #define PIN_CLK 0 

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>



    #define TFT_WHITE 0xFFFF
    #define TFT_BLACK 0x0000
    #define TFT_RED   0xF800      /* 255,   0,   0 */

#else
#include "TFT_eSPI.h"
#endif

class Display
{
  public:    

    Display(void);
    void Init();
    void StartScreen();
    void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void DrawPixel(int16_t x, int16_t y, uint16_t color);
    void DrawString(String text, int8_t line);
    void Clear(void);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t color565( uint32_t rgb);
    void color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b);
    String ConvertColor565to888hex(uint16_t color);
    int16_t height();

    protected:
#ifdef UseDMD
    MatrixPanel_I2S_DMA *display;
#else
    TFT_eSPI *display;
    TFT_eSPI tft;
#endif  

};   


#endif


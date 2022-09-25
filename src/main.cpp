#include <WiFiManager.h> 
#include <Arduino.h>

#include <time.h>  
//#include "FS.h"
#include "SD.h"
#include "Web.h"
#include "SPI.h"
#include <ArduinoOTA.h>
//#include "SPIFFS.h"
#include "video.h"
#include "main.h"
#include "web.h"

//#define firsttimeinit 5
// #define UseDMD true   // defined in platformio.ini

    #define TFT_WHITE 0xFFFF
    #define TFT_BLACK 0x0000
    #define TFT_RED   0xF800      /* 255,   0,   0 */

// flash variables
u_int8_t Flash_version=1;
bool twelveHourFormat = false;  
bool displaySeconds = false;
u_int8_t displayTime = 20;
u_int16_t TimeColor = TFT_WHITE;  
u_int16_t frameColor = TFT_RED;  


int16_t timeCounter = 0;

#ifdef firsttimeinit
void setup(void)
{
  Serial.begin(115200);
  ESPUI.prepareFileSystem();
}

void loop()
{
}
#else


#ifdef UseDMD
#define PANEL_WIDTH 64 // width: number of LEDs for 1 pannel
#define PANEL_HEIGHT 32 // height: number of LEDs
#define PANELS_NUMBER 2   // Number of horizontally chained panels 
#define PANE_WIDTH (PANEL_WIDTH*PANELS_NUMBER)
#define PANE_WIDTH_PLANE (PANE_WIDTH>>3)
#define PANE_HEIGHT PANEL_HEIGHT

// pins for SD Card: 14, 2, 15, 13

    #define PIN_R1  25
    #define PIN_G1  26
    #define PIN_B1  27
    #define PIN_R2  18  
    #define PIN_G2  21 
    #define PIN_B2  33 //2  ++++

    #define PIN_A   23
    #define PIN_B   19
    #define PIN_C   5
    #define PIN_D   22
    #define PIN_E   -1 
              
    #define PIN_LE 4 // LAT 4
    #define PIN_OE  32 //15++++
    #define PIN_CLK 0 //16

    
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSansBold18pt7b.h>

#else
//#include <TFT_eSPI.h>
//#include "gfxfont.h" // Include the header file attached to this sketch
//#include "Adafruit_GFX.h"// Hardware-specific library
//#include "Adafruit_ILI9341.h"
#include <Arduino_GFX_Library.h>
//#include <Fonts/FreeSansBold18pt7b.h>

/*
GND
VCC
SCK   18
SDA   MOSI  23
RST 22
DC  21
CS  5
*/

#define TFT_SCK    18
#define TFT_MOSI   23
#define TFT_MISO   19
#define TFT_CS     5
#define TFT_DC     21
#define TFT_RESET  22
#endif

WiFiManager wifiManager;
#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" 
const char* wifihostname = "DMD Clock";



#ifdef UseDMD
HUB75_I2S_CFG::i2s_pins _pins={PIN_R1, PIN_G1, PIN_B1, PIN_R2, PIN_G2, PIN_B2, PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_LE, PIN_OE, PIN_CLK};
HUB75_I2S_CFG mxconfig(
          PANEL_WIDTH,   // width
          PANEL_HEIGHT,   // height
           PANELS_NUMBER,   // chain length
         _pins//,   // pin mapping
         //HUB75_I2S_CFG::FM6126A      // driver chip
);
MatrixPanel_I2S_DMA *display = nullptr;

#else
Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, GFX_NOT_DEFINED /* MISO */);

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *display = new Arduino_ILI9341(bus, TFT_RESET, 3 /* rotation */, false /* IPS */);

#endif

#define TFT_GREY 0x5AEB // New colour
#define FSS9 &FreeSans9pt7b
#define FSS12 &FreeSans12pt7b

SPIClass SPISD;
File root;

byte uhrzeit[6] = {1, 2, 3, 0, 0, 0};
  int16_t last_hour = -1;
  int16_t last_min  = -1;
  int16_t last_sec  = -1;



void setup() {
  Serial.begin(115200);
  //ESPUI.prepareFileSystem();

  delay(1000);
  Serial.println("start");

#ifdef UseDMD
  display = new MatrixPanel_I2S_DMA(mxconfig);
#else
  //display = &tft;
 #endif 

  display->begin();
  #ifdef UseDMD
  display->setRotation(0); 
  #else
    display->setRotation(2); 
  #endif
  display->setTextColor(TFT_WHITE);
  display->fillScreen(TFT_BLACK);
  drawMyRect(0, 0, 128, 32, TFT_RED);

 // display->setCursor(10, 10);
 // display->setTextSize(1);
 // display->setFreeFont(FSS9);
  drawString("DMD Clock", 5, 5, 1);
  

  wifiManager.setHostname(wifihostname);
  wifiManager.autoConnect(wifihostname);

  String ipaddress = WiFi.localIP().toString();
  drawString(ipaddress.c_str(), 5, 15, 1);

  SPISD.begin(14, 2, 15, 13);//SCK MISO MOSI SS
  if (!SD.begin(13, SPISD))  {
    Serial.println("SD Card initialization failed!");
    drawString("SD Card error", 5, 5, 1);
    //while (1);
  }
  Serial.println("SD Card initialization done.");

  randomSeed(analogRead(39));

  struct tm local;
  configTzTime(MY_TZ, NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren
  Serial.println(&local, "%A, %B %d %Y %H:%M:%S");

  Web_Init();

    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();


#ifndef UseDMD
  // display->setSwapBytes(true); // We need to swap the colour bytes (endianess)
#endif

  Flash_Read();

  root = SD.open("/clips");
  if (SD.exists("/cache.txt"))
    { getCacheList("/cache.txt"); delay(1000);  }
  else
    getFilesList(root);

  display->fillScreen(TFT_BLACK);
#ifndef UseDMD
  //display->setFont(&FreeSansBold18pt7b);
#endif

}

void loopalwaysrun() {
    ArduinoOTA.handle();
}

void loop() {
  loopalwaysrun();  // also called from Video player


  if (timeCounter < displayTime) {
     DisplayTime();
  }
  else {
    //display->fillScreen(TFT_BLACK);
    playRandomVideo(); 
    timeCounter = 0;
  }  
}



void drawString(const char * thetext, int16_t x, int16_t y, int16_t font) {
#ifdef UseDMD
  display->setCursor(x, y);    // start at top left, with 8 pixel of spacing
  display->print(thetext);
#else
// display->drawString(thetext, x, y, font);
  display->setCursor(x, y);    // start at top left, with 8 pixel of spacing
  display->print(thetext);
#endif 
}

void GetTime( int16_t &hour, int16_t &min, int16_t &sec) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display->setTextColor(TFT_WHITE, 0x0000);
    drawString("Failed to obtain time",0,0);
    timeinfo.tm_hour=12;
    timeinfo.tm_min=30;
  }
  hour = timeinfo.tm_hour;
  min  = timeinfo.tm_min;
  sec = timeinfo.tm_sec;

    uhrzeit[0] = hour / 10;
    uhrzeit[1] = hour % 10;
    uhrzeit[2] = min / 10;
    uhrzeit[3] = min % 10;
    uhrzeit[4] = sec / 10;
    uhrzeit[5] = sec % 10;
}


void DisplayTime() {
  int16_t cur_hour;
  int16_t cur_min;
  int16_t cur_sec;
  GetTime( cur_hour,cur_min, cur_sec);

  if ((cur_min != last_min) | (timeCounter == 0)) {
    timeCounter++;
    fillMyRect(0, 0, 128, 32, TFT_BLACK);
    display->setTextColor(TimeColor, 0x0000);
    drawMyRect(0, 0, 128, 32, frameColor);

    char timeString [10];
#ifndef UseDMD    
    if ((cur_sec % 2) == 1)
      sprintf(timeString, "%d%d:%d%d", uhrzeit[0], uhrzeit[1], uhrzeit[2], uhrzeit[3]);
    else 
      sprintf(timeString, "%d%d %d%d", uhrzeit[0], uhrzeit[1], uhrzeit[2], uhrzeit[3]);
      // display->setFreeFont(FSS9);
      drawString(timeString, 8, 3);
#else
    display->setFont(&FreeSansBold18pt7b);
    if ((cur_sec % 2) == 1)
      sprintf(timeString, "%d%d:", uhrzeit[0], uhrzeit[1]);
    else 
      sprintf(timeString, "%d%d", uhrzeit[0], uhrzeit[1]);  
    drawString(timeString, 22, 28);
    sprintf(timeString, "%d%d", uhrzeit[2], uhrzeit[3]);
    drawString(timeString, 70, 28);   
#endif   

  }
  else
  if(cur_sec != last_sec) {
    timeCounter++;
    if ((cur_sec % 2) == 1) {
        display->setTextColor(TimeColor, 0x0000);
        drawString(":", 60, 28);
    } 
    else 
    {
      display->fillRect(60, 5, 10, 24, TFT_BLACK);
    }
  }


  last_hour = cur_hour;
  last_min = cur_min;
  last_sec = cur_sec;

}


#endif
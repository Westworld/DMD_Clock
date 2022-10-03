#include <Arduino.h>
#include <WiFiManager.h> 


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
#include "digits.h"

//#define firsttimeinit 5  // don't use that, not compatible with esp32 littlefs
// #define UseDMD true   // defined in platformio.ini

    #define TFT_WHITE 0xFFFF
    #define TFT_BLACK 0x0000
    #define TFT_RED   0xF800      /* 255,   0,   0 */

// flash variables
bool twelveHourFormat = false;  
bool displaySeconds = false;
u_int8_t displayTime = 20; 
u_int16_t frameColor = TFT_RED;  

extern String fontnames[];
extern int noFonts;

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


WiFiManager wifiManager;
#define NTP_SERVER "de.pool.ntp.org"
String MY_TZ = "CET-1CEST,M3.5.0/02,M10.5.0/03" ;
const char* wifihostname = "DMD Clock";

#ifdef UseDMD
#define PANEL_WIDTH 64 // width: number of LEDs for 1 pannel
#define PANEL_HEIGHT 32 // height: number of LEDs
#define PANELS_NUMBER 2   // Number of horizontally chained panels 
#define PANE_WIDTH (PANEL_WIDTH*PANELS_NUMBER)
#define PANE_WIDTH_PLANE (PANE_WIDTH>>3)
#define PANE_HEIGHT PANEL_HEIGHT

// pins for SD Card: 14, 2, 15, 13
// 14, 2, 15, 13);//SCK MISO MOSI SS

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

#define SD_SCK 14
#define SD_MISO 2
#define SD_MOSI 15
#define SD_SS 13
    
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
//#include <Fonts/FreeSansBold18pt7b.h>

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

#define SD_SCK  14   //grün
#define SD_MISO 27   //blau
#define SD_MOSI 13   //gelb
#define SD_SS   15   //orange

#define TFT_SCK    18  // braun
#define TFT_MOSI   23  // rot  sda
//#define TFT_MISO   19  // leer
#define TFT_CS     33 // 5   // grün
#define TFT_DC     21  // gelb
#define TFT_RESET  22  // orange

/*
GND
VCC
SCK   18
SDA   MOSI  23
RST 22
DC  21
CS  5
*/

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */

#include <Arduino_GFX_Library.h>
Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, GFX_NOT_DEFINED /* MISO */);

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *display = new Arduino_ILI9341(bus, TFT_RESET, 3 /* rotation */, false /* IPS */);

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410

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

Digits  * clockdigits;
int8_t RandomFontcounter = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("start");

#ifdef UseDMD
  display = new MatrixPanel_I2S_DMA(mxconfig);
  display->begin();
  display->setRotation(0); 
#else  
  display->begin();
#endif 

  display->setTextColor(TFT_WHITE);
  display->fillScreen(TFT_BLACK);
  drawMyRect(0, 0, 128, 32, TFT_RED);
  drawString("DMD Clock", 5, 5, 1);
  
  wifiManager.setHostname(wifihostname);
  wifiManager.autoConnect(wifihostname); 

  String ipaddress = WiFi.localIP().toString();
  drawString(ipaddress.c_str(), 5, 15, 1);
  Serial.println("vor spisd");

  SPISD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_SS);//SCK MISO MOSI SS
  if (!SD.begin(SD_SS, SPISD))  {
    Serial.println("SD Card initialization failed!");
    drawString("SD Card error", 5, 5, 1);
  }

  Serial.println("SD Card initialization done.");
  randomSeed(analogRead(39));
  struct tm local;

  if (SD.exists("/timezone.txt"))
    { MY_TZ = readTimeZone("/timezone.txt"); }

  Serial.println("TimeZone: "+MY_TZ);
  configTzTime(MY_TZ.c_str(), NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren
  Serial.println(&local, "%A, %B %d %Y %H:%M:%S");

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

  clockdigits = new Digits(display);  // we need to do this early, used in Web_init

  Serial.println("vor cache");

  if (SD.exists("/cache.txt"))
    { getCacheList("/cache.txt"); }
  else
    {
      root = SD.open("/clips");
      getFilesList(root);
      root.close();
    }
  Serial.println("vor font");

  if (SD.exists("/fontcache.txt"))
    { getFontCache("/fontcache.txt"); 
      clockdigits->fontnumber=0;
      delay(1000);  }
  else
    {
      root = SD.open("/fonts");
      getFontList(root);
      root.close();
    }

  Flash_Read();

  if (noFonts>0) {
    clockdigits->SetFontNumber(clockdigits->fontnumber);
  }

  Web_Init();
  display->fillScreen(TFT_BLACK);
  Serial.println("setup done");
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
    playRandomVideo();    

    timeCounter = 0;
    if (RandomFontcounter++ > 20) {
      RandomFontcounter=0;
      if (clockdigits->fontnumber == 0) {
        int8_t font = random(noFonts)+1;
        clockdigits->SetFontNumber(font); 
      }  
    }
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

  clockdigits->DrawTime(cur_hour,cur_min, cur_sec, timeCounter);
}


#endif
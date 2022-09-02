//#define firsttimeinit 5

#ifdef firsttimeinit
#include <ESPUI.h>

void setup(void)
{
  Serial.begin(115200);
  ESPUI.prepareFileSystem();
}

void loop()
{
}
#else

#include <WiFiManager.h> 
#include <Arduino.h>

#include <time.h>  
//#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include "SPIFFS.h"
#include <TFT_eSPI.h>
#include "gfxfont.h" // Include the header file attached to this sketch
#include "Adafruit_GFX.h"// Hardware-specific library


WiFiManager wifiManager;
#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" 
const char* wifihostname = "DMD Clock";

#include "video.h"
#include "Web.h"
#include "main.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB // New colour
#define FSS9 &FreeSans9pt7b
#define FSS12 &FreeSans12pt7b

SPIClass SPISD;
File root;

byte uhrzeit[6] = {1, 2, 3, 0, 0, 0};
  int16_t last_hour = -1;
  int16_t last_min  = -1;
  int16_t last_sec  = -1;
const bool twelveHourFormat = false;  
const bool displaySeconds = false;
int16_t displayTime = 10;
int16_t timeCounter = 0;

void setup() {
  Serial.begin(115200);
  //ESPUI.prepareFileSystem();

  delay(1000);
  Serial.println("start");

  tft.init();
  tft.begin();
  tft.setRotation(1); 
  tft.setTextColor(TFT_WHITE, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, 128, 32, TFT_RED);
 // tft.setCursor(10, 10);
 // tft.setTextSize(1);
 // tft.setFreeFont(FSS9);
  tft.drawString("DMD Clock", 5, 5, 1);

  wifiManager.setHostname(wifihostname);
  wifiManager.autoConnect(wifihostname);

  String ipaddress = WiFi.localIP().toString();
  tft.drawString(ipaddress, 5, 15, 1);

  SPISD.begin(14, 2, 15, 13);//SCK MISO MOSI SS
  if (!SD.begin(13, SPISD))  {
    Serial.println("SD Card initialization failed!");
    tft.drawString("SD Card error", 5, 5, 1);
    //while (1);
  }
  Serial.println("SD Card initialization done.");

  randomSeed(analogRead(39));

  struct tm local;
  configTzTime(MY_TZ, NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren
  Serial.println(&local, "%A, %B %d %Y %H:%M:%S");

  Web_Init();

  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  root = SD.open("/clips");
  getFilesList(root);

  delay(2000);
  tft.fillScreen(TFT_BLACK);

}


void loop() {

  if (timeCounter < displayTime)
     DisplayTime();
  else {
    tft.fillScreen(TFT_BLACK);
    playRandomVideo(); 
    timeCounter = 0;
  }  
}

void GetTime( int16_t &hour, int16_t &min, int16_t &sec) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    tft.setTextColor(TFT_WHITE, 0x0000);
    tft.drawString("Failed to obtain time",0,0);
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

  if (cur_sec != last_sec) {
    timeCounter++;

    //tft.fillScreen(TFT_BLACK);
    tft.fillRect(0, 0, 128, 32, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, 0x0000);
    tft.drawRect(0, 0, 128, 32, TFT_RED);

    char timeString [8];
    if ((cur_sec % 2) == 1)
      sprintf(timeString, "%d%d:%d%d", uhrzeit[0], uhrzeit[1], uhrzeit[2], uhrzeit[3]);
    else 
      sprintf(timeString, "%d%d %d%d", uhrzeit[0], uhrzeit[1], uhrzeit[2], uhrzeit[3]);
    tft.setFreeFont(FSS9);
    tft.drawString(timeString, 20, 5);
  }


  last_hour = cur_hour;
  last_min = cur_min;
  last_sec = cur_sec;

}

#endif
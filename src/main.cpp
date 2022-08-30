
#include <WiFiManager.h> 
#include <Arduino.h>

#include <time.h>  
//#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include "SPIFFS.h"
#include <TFT_eSPI.h>

WiFiManager wifiManager;
#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" 
const char* wifihostname = "DMD Clock";

#include "Adafruit_GFX.h"// Hardware-specific library
#include "video.h"
#include "main.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB // New colour

SPIClass SPISD;
File root;


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("start");

  wifiManager.setHostname(wifihostname);
  wifiManager.autoConnect(wifihostname);

  SPISD.begin(14, 2, 15, 13);//SCK MISO MOSI SS
  if (!SD.begin(13, SPISD))  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  randomSeed(analogRead(0));


  struct tm local;
  configTzTime(MY_TZ, NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren
  Serial.println(&local, "%A, %B %d %Y %H:%M:%S");

  tft.init();
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  root = SD.open("/clips");
  getFilesList(root);
}


void loop() {
  tft.setRotation(1);  // portrait
  tft.fillScreen(TFT_BLACK);
  
  playRandomVideo(); 
  delay(2000);
}


// refactoring, cleanup, write readme.md



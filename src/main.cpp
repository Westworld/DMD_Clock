#include <Arduino.h>
#include <WiFiManager.h> 
#include <WiFiUdp.h>

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
#include "display.h"
#include "settings.h"

#define UDPDEBUG 1
#ifdef UDPDEBUG
WiFiUDP udp;
const char * udpAddress = "192.168.0.34";
const int udpPort = 19814;
#endif

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
#define DefaultTimeZone "CET-1CEST,M3.5.0/02,M10.5.0/03"  
String MY_TZ = DefaultTimeZone ;
const char* wifihostname = "DMD Clock";

// different SD-Card pins, as my DMD ESP32 has them on board, the test ESP32 not.
#ifdef UseDMD
#define SD_SCK 14
#define SD_MISO 2
#define SD_MOSI 15
#define SD_SS 13
#else
    #ifdef TFT_PARALLEL_8_BIT
      #define SD_SCK 18
      #define SD_MISO 19
      #define SD_MOSI 23
      #define SD_SS 5
    #else
      #define SD_SCK  14   //grün
      #define SD_MISO 33   //blau
      #define SD_MOSI 13   //gelb
      #define SD_SS   15   //orange cs
    #endif  
#endif

SPIClass SPISD;
File root;

byte uhrzeit[6] = {1, 2, 3, 0, 0, 0};
  int16_t last_hour = -1;
  int16_t last_min  = -1;
  int16_t last_sec  = -1;

Digits  * clockdigits;
int8_t RandomFontcounter = 0;
Display * thedisplay;
Settings * settings;

void setTimeZone(String TimeZone) {
  struct tm local;
  configTzTime(TimeZone.c_str(), NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren
  #ifdef webdebug  
    Serial.println("TimeZone: "+TimeZone);
    Serial.println(&local, "%A, %B %d %Y %H:%M:%S");
  #endif  
 
}

void configModeCallback (WiFiManager *myWiFiManager) {
  thedisplay->DrawString("Config mode", 0);
  String ipaddress = WiFi.softAPIP().toString();
  thedisplay->DrawString(ipaddress, 1);
}

void setup() {
  Serial.begin(115200);
  #ifdef webdebug 
    Serial.println("start");
  #endif  

  settings = new Settings();
  thedisplay = new Display();
  thedisplay->StartScreen();
  
  wifiManager.setHostname(wifihostname);
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConnectRetries(10);
  wifiManager.setConnectTimeout(10);
  wifiManager.autoConnect(wifihostname); 

  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }


  String ipaddress = WiFi.localIP().toString();
  thedisplay->DrawString(ipaddress, 1);
  #ifdef webdebug 
    Serial.println("vor spisd");
  #endif  

  SPISD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_SS);//SCK MISO MOSI SS
  if (!SD.begin(SD_SS, SPISD))  {
    Serial.println("SD Card initialization failed!");
    thedisplay->DrawString("SD Card error", 1);
  }
  #ifdef webdebug 
    Serial.println("SD Card initialization done.");
  #endif  
  randomSeed(analogRead(39));

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

  clockdigits = new Digits(thedisplay, settings);  // we need to do this early, used in Web_init
  #ifdef webdebug 
    Serial.println("vor cache");
  #endif  

  if (SD.exists("/cache.txt"))
    { getCacheList("/cache.txt"); }
  else
    {
      root = SD.open("/clips");
      getFilesList(root);
      root.close();
    }
  #ifdef webdebug 
  Serial.println("vor font");
  #endif

  if (SD.exists("/fontcache.txt"))
    { getFontCache("/fontcache.txt"); 
     }
  else
    {
      root = SD.open("/fonts");
      getFontList(root);
      root.close();
    }
  #ifdef webdebug   
  Serial.println("vor timezones");
  #endif
  ReadTimeZones("/TimeZones");  

  Web_Init();  

  MY_TZ = GetCurrentTimeZone(); 
  #ifdef webdebug  
    Serial.println("Loaded TimeZone: "+MY_TZ);  
  #endif  
  setTimeZone(MY_TZ);

  thedisplay->Clear();
  #ifdef webdebug 
    Serial.println("setup done");
    UDBDebug("########setup done") ;
  #endif  
}  

void loopalwaysrun() {
      ArduinoOTA.handle();

}

void loop() {
  loopalwaysrun();  // also called from Video player

  if (timeCounter < settings->getDisplayTime()) {
     DisplayTime();
  }
  else {
    playRandomVideo();  
    settings->doRefresh(); 
    thedisplay->Clear();
    clockdigits->ResetUpDownCounter();  // if we are in the middle of drawing up down, when video started, it would continue

    timeCounter = 0;
    if (RandomFontcounter++ > 20) {
      RandomFontcounter=0;
      if (settings->getFontNumber() == 0) {
        int8_t font = random(noFonts)+1;
        settings->setFontNumber(font); 
      }  
    }
  }  
  
}



void GetTime( int16_t &hour, int16_t &min, int16_t &sec) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo, 2000)){
    thedisplay->DrawString("Failed to obtain time",0);
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


void UDBDebug(String message) {
#ifdef UDPDEBUG
  udp.beginPacket(udpAddress, udpPort);
  udp.write((const uint8_t* ) message.c_str(), (size_t) message.length());
  udp.endPacket();
#endif  
}

#endif
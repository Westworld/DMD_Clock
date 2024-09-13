#include <Arduino.h>
#include "WiFi.h"
#include <WiFiMulti.h>
//#include <WiFiManager.h> 
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <time.h>  
#include "SD.h"
#include "SPI.h"
#include "video.h"
#include "main.h"
#include "digits.h"
#include "display.h"
#include "settings.h"
#include <ArduinoSort.h>
#include <XPT2046_Touchscreen.h>

WiFiMulti wifiMulti;

// #define UDPDEBUG 1
#ifdef UDPDEBUG
WiFiUDP udp;
const char * udpAddress = "192.168.0.63";
const int udpPort = 19814;
#endif

extern String fontnames[];
extern int noFonts;
#define maxtimezonenames 15
#define maxtimezonecitynames 150
String timezonenames[maxtimezonenames];
uint8_t notimezonenames = 0;
String timezonencityvalues[maxtimezonecitynames];
String timezonencitynames[maxtimezonecitynames];
uint16_t timezonencityids[maxtimezonecitynames];
uint8_t notimezonecitynames = 0;
uint16_t SelectIDTimeZoneCity = 0;

int16_t timeCounter = 0;

uint8_t configmode = 0;
uint32_t configmodestart = 0;
char logString[200];

//WiFiManager wifiManager;
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
  #ifdef UseCYD
      #define SD_SCK 18
      #define SD_MISO 19
      #define SD_MOSI 23
      #define SD_SS 5
      #define XPT2046_IRQ 36
      #define XPT2046_MOSI 32
      #define XPT2046_MISO 39
      #define XPT2046_CLK 25
      #define XPT2046_CS 33
      SPIClass touchscreenSpi = SPIClass(VSPI);
      XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
      uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240,touchScreenMaximumY = 3800;

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



void ConnectWifi(bool redirectWifi1) {
    if (redirectWifi1) {
      WiFi.disconnect();
      WiFi.mode(WIFI_STA);
      WiFi.setHostname(wifihostname);
      wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
    }
    else {
      WiFi.mode(WIFI_STA);
      WiFi.setHostname(wifihostname);
      wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
      wifiMulti.addAP(WIFI_SSID3, WIFI_PASS3); 
      wifiMulti.addAP(WIFI_SSID2, WIFI_PASS2);
    }

    int loop=1;

   thedisplay->DrawString("Connecting Wifi...",0);

    while ((loop < 10) && (wifiMulti.run() != WL_CONNECTED)) {
      thedisplay->DrawString("try again to connect",1);
      delay(1000);
    }
    if(loop < 10) {
         thedisplay->DrawString("connected           ",1);
    }
    else
    {
           thedisplay->DrawString("NOT CONNECTED ********+",1);
           delay(5000);
    }
}


void setup() {
  Serial.begin(115200);
  #ifdef webdebug 
    delay(1000);
    Serial.println("start");
  #endif  

  settings = new Settings();
    #ifdef webdebug 
    Serial.println("Settings loaded");
  #endif  
  thedisplay = new Display();
  thedisplay->StartScreen();
  
 ConnectWifi(false);

  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }


  String ipaddress = WiFi.localIP().toString();
  thedisplay->DrawString(ipaddress, 1);
  #ifdef webdebug 
    Serial.println("before spisd");
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


  clockdigits = new Digits(thedisplay, settings);  // we need to do this early, used in Web_init
  #ifdef webdebug 
    Serial.println("before cache");
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
  Serial.println("before font");
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
  Serial.println("before timezones");
  #endif
  ReadTimeZones("/TimeZones");   

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

#ifdef UseCYD
  #define CYD_LED_BLUE 17
  #define CYD_LED_RED 4
  #define CYD_LED_GREEN 16
  pinMode(CYD_LED_RED, OUTPUT);  // all off
  pinMode(CYD_LED_GREEN, OUTPUT);
  pinMode(CYD_LED_BLUE, OUTPUT);
  digitalWrite(CYD_LED_RED, HIGH); 
  digitalWrite(CYD_LED_GREEN, HIGH);
  digitalWrite(CYD_LED_BLUE, HIGH);

  touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); /* Start second SPI bus for touchscreen */
  touchscreen.begin(touchscreenSpi); /* Touchscreen init */
  touchscreen.setRotation(1); /* Inverted landscape orientation to match screen */

#endif
}  

void loopalwaysrun() {

}

short checkButton() {
// 0 == no button, 1 = short, 2 = long

  #ifdef UseCYD
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    uint32_t timer = millis();

    while (touchscreen.tirqTouched() && touchscreen.touched()) 
      ;  // loop

    if (millis()-timer > 1000)
      return 2;
    else 
      return 1;
  }
  else
    return 0;

  #else
    return 0;
  #endif

}

void handleClick(short button) {
  // 1 = short, 2 = long
  #ifdef webdebug 
    Serial.println("Button"); 
  #endif

  if (configmode == 0) {
      configmode = 1;
      thedisplay->Clear();
      thedisplay->DrawRect(0, 0, 128, 32, TFT_RED);
      if (WiFi.status() != WL_CONNECTED) {
          ConnectWifi(false);
      }
      thedisplay->DrawString("Config mode", 0);
      String ipaddress = WiFi.localIP().toString();
      thedisplay->DrawString(ipaddress, 1);

      ArduinoOTA.setHostname(wifihostname);

      ArduinoOTA
        .onStart([]() {
          thedisplay->DrawString("Start updating", 0);
          Serial.println("Start updating");
        });
        ArduinoOTA.onEnd([]() {
          thedisplay->DrawString("End updating", 0);
          Serial.println("End updating");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          sprintf(logString, "Progress: %u", (progress / (total / 100)));
          Serial.println(logString);
          thedisplay->DrawString(logString, 1);
        });
        ArduinoOTA.onError([](ota_error_t error) {
          sprintf(logString, "Error[%u]: ", error);
          Serial.println(logString);
          thedisplay->DrawString(logString, 1);
          if (error == OTA_AUTH_ERROR)  Serial.println("Auth Failed");
          else if (error == OTA_BEGIN_ERROR)  Serial.println("Begin Failed");
          else if (error == OTA_CONNECT_ERROR)  Serial.println("Connect Failed");
          else if (error == OTA_RECEIVE_ERROR)  Serial.println("Receive Failed");
          else if (error == OTA_END_ERROR)  Serial.println("End Failed");
        });

      ArduinoOTA.begin();  

      configmodestart = millis();
  }
  else {
    // already in config mode
      if (button == 2) {
        // long click, reboot
        ESP.restart();
      }
      if ((millis() - configmodestart) > 10000) {
        // switch to Wifi1 only, main network
        ConnectWifi(true);
      }
      else 
      {  // leave config mode
        configmode = 0;
        ArduinoOTA.end();
        thedisplay->Clear();
        thedisplay->DrawRect(0, 0, 128, 32, TFT_RED);
      }
      // force redraw
  }

}

void loop() {
  loopalwaysrun();  // also called from Video player

  int click = checkButton();
  if (click != 0) handleClick(click);

  if (configmode == 0) {

      if (timeCounter < settings->getDisplayTime()) {
        DisplayTime();
      }
      else {
        playRandomVideo();  
        if (settings->getClockBlend()) {
          // nothing
        }
        else
        {
          settings->doRefresh(); 
          thedisplay->Clear();
          clockdigits->ResetUpDownCounter();  // if we are in the middle of drawing up down, when video started, it would continue
        }
        
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
  else {
    // in config mode
      ArduinoOTA.handle();
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

void VideoRedrawTime() {
  clockdigits->RedrawTime();
}


void UDBDebug(String message) {
#ifdef UDPDEBUG
  udp.beginPacket(udpAddress, udpPort);
  udp.write((const uint8_t* ) message.c_str(), (size_t) message.length());
  udp.endPacket();
#endif  
}


 void ReadTimeZones(String path) {
  File dir = SD.open(path);
  String dirchar = "/";
  while (true)
  {
    File entry =  dir.openNextFile();
    short filetype=0;
    if (! entry)
    {
      // no more files
      break;
    }
    
    if (entry.isDirectory()) 
    {
      // Skip file if in subfolder
       entry.close(); // Close folder entry
    } 
    else
    {
      //Serial.println(entry.name());
      String dirname = dir.name();
      String filename = entry.name();

      if (filename.startsWith(".")) continue;
      if (!(filename.endsWith(".txt"))) continue;
      filename = filename.substring(0,filename.length()-4);

      timezonenames[notimezonenames++] = filename;

#ifdef webdebug
  Serial.println("TZ "+filename+" nr: "+String(notimezonenames));
#endif

      entry.close();
      if (notimezonenames>=maxtimezonenames)
        { notimezonenames--; break;}
    }
      sortArray(timezonenames, notimezonenames);

  }
  dir.close();
 }

String GetCurrentTimeZone() {
  u_int8_t area = settings->getTimeZoneArea();
  if ((area < 0) && (area >notimezonenames))
    area = 0;  

  if (notimezonecitynames < 1)  
    GetTimeZoneNames(timezonenames[area]);

  uint8_t id = settings->getTimeZoneID();
  if (id >= notimezonecitynames) 
    #ifdef webdebug
      Serial.println("TZ error. Was "+String(id)+" max names: "+String(notimezonecitynames));
    #endif
  {  id = 0; settings->setTimeZone(id); }

  return timezonencityvalues[id];
}

void GetTimeZoneNames(String path) {
  File card;
  String zonename, zoneTZ;

#ifdef webdebug
  Serial.println(path);
#endif

  path = "/TimeZones/"+path+".txt";
  notimezonecitynames = 0;
  card = SD.open(path);
  if(card) {
    while(card.available()) {
      zonename = card.readStringUntil('\t');
      zoneTZ = card.readStringUntil('\n');
      if ((zoneTZ.endsWith("\r"))) zoneTZ.remove(zoneTZ.length()-1);

      #ifdef webdebug
      Serial.println("Add city ("+String(notimezonecitynames)+") "+zonename);
      #endif 

      timezonencityvalues[notimezonecitynames] = zoneTZ;
      timezonencitynames[notimezonecitynames] = zonename;

      notimezonecitynames++;      
    }
    card.close();
  }

}

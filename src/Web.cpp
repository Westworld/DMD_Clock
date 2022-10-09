#include "Web.h"
#include "Video.h"
#include "digits.h"
#include <ArduinoSort.h>
#include "display.h"

#define TFT_WHITE 0xFFFF

void setTimeZone(String TimeZone);
extern String fontnames[];
extern int noFonts;
String timezonenames[maxtimezonenames];
uint8_t notimezonenames = 0;
String timezonencityvalues[maxtimezonecitynames];
String timezonencitynames[maxtimezonecitynames];
uint16_t timezonencityids[maxtimezonecitynames];
uint8_t notimezonecitynames = 0;
uint16_t SelectIDTimeZoneCity = 0;
extern String MY_TZ;

extern Digits  * clockdigits;
extern Display * thedisplay;
extern Settings * settings;


void Web_timedisplayCall(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Time Display: "+sender->value, 0);
  #endif
  settings->setDisplayTime(sender->value.toInt());
}

void Web_timecolorCall(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Time Color: "+sender->value, 0);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  settings->setFontColor(thedisplay->color565(color));
}

void Web_timeframeCall(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Frame Color: "+sender->value, 0);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  settings->setFrameColor(thedisplay->color565(color));
}

void Web_FontCall(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Font Call: "+sender->value, 0);
    Serial.println("Font Call: "+sender->value);
  #endif

  String message = sender->value;
  settings->setFontNumber(strtol(message.c_str(), 0, 10));
}

void WebSwitchSeconds(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        settings->setDisplaySeconds(true);
        break;

    case S_INACTIVE:
        settings->setDisplaySeconds(false);
        break;
    }
}

void WebSwitchAMPM(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        settings->setTwelveHourFormat(true);
        break;

    case S_INACTIVE:
        settings->setTwelveHourFormat(false);
        break;
    }
}

void Web_TimeZoneArea(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Web_TimeZoneArea Call: "+sender->value, 0);
    Serial.println("Web_TimeZoneArea Call: "+sender->value);
  #endif

  String message = sender->value;
  u_int8_t zone = strtol(message.c_str(), 0, 10);
  settings->setTimeZone(zone, 0);
   // delete all entries
  for (int i=0; i<notimezonecitynames; i++) {
      ESPUI.removeControl(timezonencityids[i], true);
  }
  Web_AddTimeZoneNames(SelectIDTimeZoneCity, timezonenames[zone]);
}

void Web_TimeZoneSelect(Control* sender, int type)
{
  #ifdef webdebug
    thedisplay->DrawString("Web_TimeZoneSelect Call: "+sender->value, 0);
    Serial.println("Web_TimeZoneSelect Call: "+sender->value);
  #endif

  String message = sender->value;
  u_int8_t id = strtol(message.c_str(), 0, 10);
  settings->setTimeZone(id);
  MY_TZ = timezonencityvalues[id];
  setTimeZone(timezonencityvalues[id]);
}


void Web_Init() {
  ESPUI.number("Time display in seconds:", &Web_timedisplayCall, ControlColor::Alizarin, settings->getDisplayTime());
  String colorstring = thedisplay->ConvertColor565to888hex(settings->getFontColor());
  uint16_t text_colour = ESPUI.text("Time display color:", &Web_timecolorCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  colorstring = thedisplay->ConvertColor565to888hex(settings->getFrameColor());
  text_colour = ESPUI.text("Time frame color:", &Web_timeframeCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  uint16_t select1
        = ESPUI.addControl(ControlType::Select, "Font:", "", ControlColor::Alizarin, -1, &Web_FontCall);
  ESPUI.addControl(ControlType::Option, "Random", "0", ControlColor::Alizarin, select1);
  for (int i=0; i<noFonts; i++)    
      ESPUI.addControl(ControlType::Option, fontnames[i].c_str(), String(i+1), ControlColor::Alizarin, select1);

  ESPUI.updateSelect(select1, String(settings->getFontNumber()), -1);

  ESPUI.switcher("Show Seconds", &WebSwitchSeconds, ControlColor::Alizarin, settings->getDisplaySeconds());
  ESPUI.switcher("12 hour format", &WebSwitchAMPM, ControlColor::Alizarin, settings->getTwelveHourFormat());

  select1 = ESPUI.addControl(ControlType::Select, "Time Zone Continent:", "", ControlColor::Alizarin, -1, &Web_TimeZoneArea);
  for (int i=0; i<notimezonenames; i++)    
      ESPUI.addControl(ControlType::Option, timezonenames[i].c_str(), String(i), ControlColor::Alizarin, select1);
  u_int8_t area = settings->getTimeZoneArea();
  ESPUI.updateSelect(select1, String(area), -1);

  SelectIDTimeZoneCity = ESPUI.addControl(ControlType::Select, "Time Zone:", "", ControlColor::Alizarin, -1, &Web_TimeZoneSelect);
  Web_AddTimeZoneNames(SelectIDTimeZoneCity, timezonenames[area]);

  ESPUI.begin("DMD Clock");
}

void Web_AddTimeZoneNames(uint16_t select, String path) {
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

      timezonencityids[notimezonecitynames] = ESPUI.addControl(ControlType::Option, timezonencitynames[notimezonecitynames].c_str(), String(notimezonecitynames), ControlColor::Alizarin, select);
      notimezonecitynames++;      
    }
    uint8_t id = settings->getTimeZoneID();
    if (notimezonecitynames >= id)
        ESPUI.updateSelect(select, String(id), -1);
    card.close();
  }

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

//#ifdef webdebug
//Serial.println("TZ "+filename+" nr: "+String(notimezonenames));
//#endif

      entry.close();
      if (notimezonenames>=maxtimezonenames)
        { notimezonenames--; break;}
    }
    sortArray(timezonenames, notimezonenames);
  }
  dir.close();
 }

String GetCurrentTimeZone() {
  uint8_t id = settings->getTimeZoneID();
  if (id >= notimezonecitynames) 
  {  id = 0; settings->setTimeZone(id); }

  return timezonencityvalues[id];
}

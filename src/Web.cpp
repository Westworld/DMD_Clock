#include "Web.h"
#include "Video.h"
#include "EEPROM.h"
#include "digits.h"
#include <ArduinoSort.h>

void setTimeZone(String TimeZone);

extern u_int8_t Flash_version;
extern bool twelveHourFormat;  
extern bool displaySeconds ;
extern u_int8_t displayTime ; 
extern u_int16_t frameColor; 
u_int8_t timezonearea=8;  // Europe
u_int8_t timezoneid=0; // first entry

extern String fontnames[];
extern int noFonts;

#define TFT_WHITE 0xFFFF


String timezonenames[maxtimezonenames];
uint8_t notimezonenames = 0;
String timezonencityvalues[maxtimezonecitynames];
String timezonencitynames[maxtimezonecitynames];
uint16_t timezonencityids[maxtimezonecitynames];
uint8_t notimezonecitynames = 0;
uint16_t SelectIDTimeZoneCity = 0;
extern String MY_TZ;

extern Digits  * clockdigits;

extern int16_t timeCounter;

#define EEPROM_SIZE 12

void Flash_Read() {
  int8_t font;
  // check if our structure
  EEPROM.begin(EEPROM_SIZE);
  int8_t check = EEPROM.read(0);

  if (check != 0x4D) 
    Flash_Write(0x4D);
  else {
    int8_t version = EEPROM.read(1);
 
    switch (version) {
      case 1:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        clockdigits->fontcolor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);     
        clockdigits->SetFontNumber(0);  
        Flash_Write(0x4D); 
        break;

      case 2:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        clockdigits->fontcolor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);  
        font = EEPROM.read(9);           
        clockdigits->SetFontNumber(font);   
        Flash_Write(0x4D); 
       break;     

      case 3:
        displayTime = EEPROM.read(2);
        twelveHourFormat = EEPROM.read(3);
        displaySeconds = EEPROM.read(4);
        clockdigits->fontcolor = EEPROM.readShort(5);
        frameColor = EEPROM.readShort(7);  
        font = EEPROM.read(9);           
        clockdigits->SetFontNumber(font);  
        timezonearea = EEPROM.read(10);
        timezoneid = EEPROM.read(11);
        break;   

      default:
        Flash_Write(0x4D);
    }
  }
  EEPROM.end();

  Serial.println("FlashRead TZ="+String(timezonearea)+"/"+String(timezoneid)+" displayTime="+String(displayTime)+" font="+String(clockdigits->fontnumber)+" fontcolor="+String(clockdigits->fontcolor));
  if (clockdigits->fontcolor == 0)
    clockdigits->fontcolor = TFT_WHITE;


}

void Flash_Write(int8_t what) {
  EEPROM.begin(EEPROM_SIZE);
  Serial.print("what: ");
  Serial.println(what); 
 switch(what) {
   case 0x4D:  // all
    DebugString("Flash Init");
    delay(2000);
    EEPROM.write(0, 0x4D);
    EEPROM.write(1, 3);  // version
    EEPROM.write(2, displayTime);
    EEPROM.write(3, twelveHourFormat);
    EEPROM.write(4, displaySeconds);
    EEPROM.writeShort(5, clockdigits->fontcolor);
    EEPROM.writeShort(7, frameColor);
    EEPROM.write(9, clockdigits->fontnumber);  
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);  
    break;

   case 2:
    EEPROM.write(2, displayTime); break;
   case 3:
    EEPROM.write(3, twelveHourFormat); break;       
   case 4:
    EEPROM.write(4, displaySeconds); break;
   case 5:
    EEPROM.writeShort(5, clockdigits->fontcolor); break;   
   case 7:
    EEPROM.writeShort(7, frameColor); break;
   case 9:
    EEPROM.write(9, clockdigits->fontnumber);
    break;  
   case 10:
    EEPROM.write(10, timezonearea);  
    EEPROM.write(11, timezoneid);    
    break;
} 

   EEPROM.commit();
   EEPROM.end();
}

void color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b){
  r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  b = (((color & 0x1F) * 527) + 23) >> 6;
}

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t color565( uint32_t rgb)
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

String ConvertColor565to888hex(uint16_t color) {
  uint8_t r, g, b;
  color565to888(color, r, g, b);
  char hexout[8];
  sprintf(hexout,"#%02x%02x%02x",r,g,b);
  String result = hexout;
  return result;
}  


void Web_timedisplayCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Time Display: "+sender->value);
  #endif

  displayTime = sender->value.toInt();
  timeCounter = 0;
  Flash_Write(2);
}

void Web_timecolorCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Time Color: "+sender->value);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  clockdigits->fontcolor= color565(color);
  timeCounter = 0;
  Flash_Write(5);
}

void Web_timeframeCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Frame Color: "+sender->value);
  #endif

  String message = sender->value.substring(1);
  long color = strtol(message.c_str(), 0, 16); 
  frameColor= color565(color);
  timeCounter = 0;
  Flash_Write(7);
}

void Web_FontCall(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Font Call: "+sender->value);
    Serial.println("Font Call: "+sender->value);
  #endif

  String message = sender->value;
  long font = strtol(message.c_str(), 0, 10); 
  clockdigits->SetFontNumber(font);
  timeCounter = 0;
  Flash_Write(9);
}

void WebSwitchSeconds(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("Seconds Active:");
        displaySeconds = true;
        break;

    case S_INACTIVE:
        Serial.print("Seconds Inactive");
        displaySeconds = false;
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);

    timeCounter = 0;
    Flash_Write(4);
}

void WebSwitchAMPM(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        Serial.print("12 Hour Active:");
        twelveHourFormat = true;
        break;

    case S_INACTIVE:
        Serial.print("12 Hour Inactive");
        twelveHourFormat = false;
        break;
    }

    Serial.print(" ");
    Serial.println(sender->id);

    timeCounter = 0;
    Flash_Write(3);
}

void Web_TimeZoneArea(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Web_TimeZoneArea Call: "+sender->value);
    Serial.println("Web_TimeZoneArea Call: "+sender->value);
  #endif

  String message = sender->value;
  timezonearea = strtol(message.c_str(), 0, 10); 
  timezoneid = 0;
  Flash_Write(10);
  timeCounter = 0;
  // delete all entries
  for (int i=0; i<notimezonecitynames; i++) {
      ESPUI.removeControl(timezonencityids[i], true);
  }
  Web_AddTimeZoneNames(SelectIDTimeZoneCity, timezonenames[timezonearea]);
}

void Web_TimeZoneSelect(Control* sender, int type)
{
  #ifdef webdebug
    DebugString("Web_TimeZoneSelect Call: "+sender->value);
    Serial.println("Web_TimeZoneSelect Call: "+sender->value);
  #endif

  String message = sender->value;
  timezoneid = strtol(message.c_str(), 0, 10);
  timeCounter = 0;
  Flash_Write(10);
  MY_TZ = timezonencityvalues[timezoneid];
  setTimeZone(timezonencityvalues[timezoneid]);
}


void Web_Init() {
  ESPUI.number("Time display in seconds:", &Web_timedisplayCall, ControlColor::Alizarin, displayTime);
  String colorstring = ConvertColor565to888hex(clockdigits->fontcolor);
  uint16_t text_colour = ESPUI.text("Time display color:", &Web_timecolorCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  colorstring = ConvertColor565to888hex(frameColor);
  text_colour = ESPUI.text("Time frame color:", &Web_timeframeCall, ControlColor::Alizarin, colorstring);
  ESPUI.setInputType(text_colour, "color");

  uint16_t select1
        = ESPUI.addControl(ControlType::Select, "Font:", "", ControlColor::Alizarin, -1, &Web_FontCall);
  ESPUI.addControl(ControlType::Option, "Random", "0", ControlColor::Alizarin, select1);
  for (int i=0; i<noFonts; i++)    
      ESPUI.addControl(ControlType::Option, fontnames[i].c_str(), String(i+1), ControlColor::Alizarin, select1);

  ESPUI.updateSelect(select1, String(clockdigits->fontnumber), -1);

  ESPUI.switcher("Show Seconds", &WebSwitchSeconds, ControlColor::Alizarin, displaySeconds);
  ESPUI.switcher("12 hour format", &WebSwitchAMPM, ControlColor::Alizarin, twelveHourFormat);

  select1 = ESPUI.addControl(ControlType::Select, "Time Zone Continent:", "", ControlColor::Alizarin, -1, &Web_TimeZoneArea);
  for (int i=0; i<notimezonenames; i++)    
      ESPUI.addControl(ControlType::Option, timezonenames[i].c_str(), String(i), ControlColor::Alizarin, select1);
  //Serial.println("TZ select: " + timezonenames[timezonearea])   ; 
  ESPUI.updateSelect(select1, String(timezonearea), -1);

  SelectIDTimeZoneCity = ESPUI.addControl(ControlType::Select, "Time Zone:", "", ControlColor::Alizarin, -1, &Web_TimeZoneSelect);
  Web_AddTimeZoneNames(SelectIDTimeZoneCity, timezonenames[timezonearea]);

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
    if (notimezonecitynames >= timezoneid)
        ESPUI.updateSelect(select, String(timezoneid), -1);
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
  if (timezoneid >= notimezonecitynames) 
    timezoneid = 0;

  return timezonencityvalues[timezoneid];
}

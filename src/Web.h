// GUI
#include <FS.h>
#include <LITTLEFS.h> 
#include <ESPUI.h>

#define maxtimezonenames 15
#define maxtimezonecitynames 150

void Web_Init();
void Flash_Read();
void Flash_Write(int8_t what);
void Web_AddTimeZoneNames(uint16_t select, String path);
void ReadTimeZones(String path) ;
String GetCurrentTimeZone();
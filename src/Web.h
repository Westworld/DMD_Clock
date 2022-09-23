// GUI
#include <FS.h>
#include <LITTLEFS.h> 
#include <ESPUI.h>

void Web_Init();
void Flash_Read();
void Flash_Write(int8_t what);
void color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b);
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
String ConvertColor565to888hex(uint16_t color);
uint16_t color565(uint32_t rgb32) ;
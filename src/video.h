#include "JPEGDEC.h"
#include "MjpegClass.h"
#include "SD.h"

#ifdef UseDMD
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#else
#include "Adafruit_GFX.h"// Hardware-specific library
#include "Adafruit_ILI9341.h"
#endif

void * myOpen(const char *filename, int32_t *size);
void myClose(void *handle);
int32_t myRead(JPEGFILE *handle, uint8_t *buffer, int32_t length);
int32_t mySeek(JPEGFILE *handle, int32_t position);

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
int drawMCUs(JPEGDRAW *pDraw);
int getNoFiles(File dir, int numTabs);
void drawimage(char * name);
void PlayVideo(String name);
void getFilesList(File dir);
void playRandomVideo() ;

#ifdef UseDMD

void drawImg(int x, int y, int width, int height, uint16_t* bitmap) ;
#endif
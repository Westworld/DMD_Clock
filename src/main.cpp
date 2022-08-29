
//#include <WiFiManager.h> 
#include <Arduino.h>

#include <time.h>  
//#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include "SPIFFS.h"
#include <TFT_eSPI.h>

#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" 
const char* wifihostname = "DMD Clock";

#include "Adafruit_GFX.h"// Hardware-specific library
//#include "TJpg_Decoder.h"
#include "JPEGDEC.h"
#include "main.h"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB // New colour

SPIClass SPISD;
JPEGDEC jpeg;

File myfile, root;

void * myOpen(const char *filename, int32_t *size) {
  myfile = SD.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle) {
  if (myfile) myfile.close();
}
int32_t myRead(JPEGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!myfile) return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(JPEGFILE *handle, int32_t position) {
  if (!myfile) return 0;
  return myfile.seek(position);
}

/* MJPEG Video */
#define FPS 24
#define MJPEG_BUFFER_SIZE (160 * 128 * 2 / 4)
#include "MjpegClass.h"
static MjpegClass mjpeg;
int noFiles = 0; // Number of media files on SD Card in clips directory
#define maxFiles 100
String filenames[maxFiles];
short nextclipid=-1;

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

int drawMCUs(JPEGDRAW *pDraw)
{
  int iCount;
  iCount = pDraw->iWidth * pDraw->iHeight; // number of pixels to draw in this call

  tft.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  return 1; // returning true (1) tells JPEGDEC to continue decoding. Returning false (0) would quit decoding immediately.
} /* drawMCUs() */

// pixel drawing callback
static int drawMCU(JPEGDRAW *pDraw)
{
  //gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  tft.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  return 1;
}

int getNoFiles(File dir, int numTabs)
{  
  while (true)
  {
    File entry =  dir.openNextFile();
    if (! entry)
    {
      // no more files
      break;
    }
    
    //for (uint8_t i = 0; i < numTabs; i++)
    //{
    //  Serial.print('\t');
    //}
    
    if (entry.isDirectory()) 
    {
      // Skip file if in subfolder
       entry.close(); // Close folder entry
    } 
    else
    {
      //Serial.println(entry.name());
      String dirname = dir.name();
      String dirchar = "/";
      String filename = entry.name();
      if (filename.startsWith(".")) continue;
      if (!(filename.endsWith(".mjpeg"))) continue;
      filename = dirchar + dirname + dirchar + filename;
      Serial.println("filename: "+filename);

      filenames[noFiles++] = filename;
      entry.close();
      if (noFiles>maxFiles)
        return noFiles-1;
    }
  }
  return noFiles;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("start");

  SPISD.begin(14, 2, 15, 13);//SCK MISO MOSI SS
  if (!SD.begin(13, SPISD))  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  randomSeed(analogRead(0));


  struct tm local;
  //configTzTime(MY_TZ, NTP_SERVER); // ESP32 Systemzeit mit NTP Synchronisieren
  //getLocalTime(&local, 10000);      // Versuche 10 s zu Synchronisieren

  tft.init();
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  root = SD.open("/clips");
  noFiles = getNoFiles(root,0);
}

void drawimage(char * name) {
  unsigned long lTime;
  lTime = micros();
     jpeg.open((const char *)name, myOpen, myClose, myRead, mySeek, drawMCUs);
        jpeg.decode(0, 0, 0);
        jpeg.close();
  lTime = micros() - lTime;
  Serial.println(lTime);

}

void PlayVideo(String name) {
    int next_frame = 0;
    int skipped_frames = 0;  
    unsigned long total_play_audio = 0;
    unsigned long total_read_video = 0;
    unsigned long total_decode_video = 0;
    unsigned long start_ms, curr_ms, next_frame_ms;

    Serial.println("Play: "+name);
    File vFile = SD.open(name);
    //Serial.println("Created vFile!");

    uint8_t *mjpeg_buf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);
    if (!mjpeg_buf)
    {
      Serial.println(F("mjpeg_buf malloc failed!"));  
    }
    else
    {
      // init Video
      mjpeg.setup(&vFile, mjpeg_buf, drawMCU, false, false); //MJPEG SETUP -> bool setup(Stream *input, uint8_t *mjpeg_buf, JPEG_DRAW_CALLBACK *pfnDraw, bool enableMultiTask, bool useBigEndian)
    }

  if (!vFile || vFile.isDirectory())
    {
      Serial.println(("ERROR: Failed to open .mjpeg file for reading"));
    }
    else
    {

      start_ms = millis();
      curr_ms = start_ms;
      next_frame_ms = start_ms + (++next_frame * 1000 / FPS);
        while ((vFile.available()) && (skipped_frames<10))
        {
          // Read video
          mjpeg.readMjpegBuf();
          total_read_video += millis() - curr_ms;
          curr_ms = millis();

          if (millis() < next_frame_ms) // check show frame or skip frame
          {
            // Play video
            mjpeg.drawJpg();
            total_decode_video += millis() - curr_ms;
          }
          else
          {
            ++skipped_frames;
            Serial.println(F("Skip frame"));
          }
          curr_ms = millis();

          next_frame_ms = start_ms + (++next_frame * 1000 / FPS);
        }

        int time_used = millis() - start_ms;
        int total_frames = next_frame - 1;
       // Serial.println(F("MJPEG video end"));
        vFile.close();
        mjpeg.reset();
    }

    if (mjpeg_buf)
   {
        free(mjpeg_buf);
    }
}

void loop() {
  
  // Fill screen with random colour so we can see the effect of printing with and without 
  // a background colour defined
  
// durch Ordner Clips laufen, jeden Film abspielen

  tft.setRotation(1);  // portrait
  tft.fillScreen(TFT_BLACK);
  
  short next = random(noFiles);
  while(nextclipid == next) {
    next = random(noFiles);
  }
  nextclipid = next;

  String name = filenames[nextclipid];

   tft.fillScreen(TFT_BLACK);
   PlayVideo( name);
   delay(2000);
}


// refactoring, cleanup, write readme.md



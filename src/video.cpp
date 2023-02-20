// #define UseDMD true  // defined in platformio.ini
#define ImageTest true

#include "video.h"
#include "display.h"
#include "settings.h"

void UDBDebug(String message);

/* MJPEG Video */
#define FPS 30
#define MJPEG_BUFFER_SIZE (160 * 128 * 2 / 4)

static MjpegClass mjpeg;
int noFiles = 0; // Number of media files on SD Card in clips directory
#define maxFiles 500
String filenames[maxFiles];
int filetypes[maxFiles];
#define maxFontNames 100
int noFonts = 0;
String fontnames[maxFontNames];
short nextclipid=-1;
File myfile;
JPEGDEC jpeg;


#ifdef ImageTest
unsigned char buffer[12288];
#endif

extern Display * thedisplay;
extern Settings * settings;
extern void loopalwaysrun();

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

void drawImg(int x, int y, int width, int height, uint16_t* bitmap) 
{
  for (int yy=0; yy<height; yy++) {
    for (int xx=0; xx<width; xx++) {
        thedisplay->DrawPixel(xx+x, yy+y, bitmap[(yy*width+xx)]);
    }
  }  
}

void drawMyString(int16_t x, int16_t y, char * text, char* fontbuffer, char* glyphs) {
  if (fontbuffer == NULL) return;

  // lieber klasse für Clock schreiben?
  /*
init mit Display
setfont mit namen/index
draw - text.

Oder methode setfont, definiert fontbuffer und glpyphs. Freigabe bei exit oder neuaufruf

  */
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= thedisplay->height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
 drawImg(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

int drawMCUs(JPEGDRAW *pDraw)
{
  int iCount;
  iCount = pDraw->iWidth * pDraw->iHeight; // number of pixels to draw in this call
  drawImg(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  return 1; // returning true (1) tells JPEGDEC to continue decoding. Returning false (0) would quit decoding immediately.
} /* drawMCUs() */

// pixel drawing callback
static int drawMCU(JPEGDRAW *pDraw)
{
  //gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  drawImg(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
  return 1;
}

int getVideoFiles(File dir, int numTabs)
{  
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

      filetype=0;
      if ((filename.endsWith(".mjpeg"))) filetype=1;
      if ((filename.endsWith(".rgb"))) filetype=2;
      if ((filename.endsWith(".bgr"))) filetype=3;
     
      if (filetype == 0) continue;

      filename = dirchar + dirname + dirchar + filename;
      #ifdef webdebug 
      Serial.println("filename: "+filename);
      #endif

      filetypes[noFiles] = filetype;
      filenames[noFiles++] = filename;
      entry.close();
      if (noFiles>=maxFiles)
        { noFiles--; break;}
    }
  }

  // create cache
  thedisplay->DrawString("create new cache", 0);
  File card;
  card = SD.open("/cache.txt",FILE_WRITE);
  for (short i=0; i<noFiles;i++) {
    card.println(filenames[i]);
  }
  card.close();
  return noFiles;
}


int getFontFiles(File dir, int numTabs)
{  
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
      if (!filename.endsWith(".font")) continue;
     
      filename = dirchar + dirname + dirchar + filename;
      #ifdef webdebug 
      Serial.println("filename: "+filename);
      #endif

      fontnames[noFonts++] = filename;
      entry.close();
      if (noFonts>=maxFontNames)
        { noFonts--; break;}
    }
  }

  // create cache
  thedisplay->DrawString("create new font cache", 0);
  File card;
  card = SD.open("/fontcache.txt",FILE_WRITE);
  for (short i=0; i<noFonts;i++) {
    card.println(fontnames[i]);
  }
  card.close();
  return noFonts;
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

#ifdef webdebug 
    Serial.println("Play: "+name);
#endif    
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
          // here the script misses a delay!
          //while (millis() < (next_frame_ms-50))
          //  yield();
            
          //start_ms += 250;
          //curr_ms = millis();

          next_frame_ms = start_ms + (++next_frame * 1000 / FPS);
          loopalwaysrun();
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


void PlayRawVideo(String name, short filetype) {
  #define rawsize 12288
#ifdef webdebug 
  Serial.println("Play: "+name);
  Serial.print("Filetype: ");
  Serial.println(filetype);
  UDBDebug("Play: "+name);
#endif

  int32_t size;
  myfile = SD.open(name);
  if (!myfile) {
   Serial.println("file not found");
   return;
  }
  size = myfile.size();
  #ifdef webdebug 
    if (size < 300000) {
      Serial.println("size: "+String(size));
      UDBDebug("size: "+String(size));
    }
  #endif

  uint8_t *buffer = (uint8_t *)malloc(rawsize);
  if (!buffer) return;
  //uint16_t *memory = (uint16_t *)malloc(128*32*2);
  uint16_t *memory = (uint16_t *) calloc(128*32, sizeof(uint16_t));
  if (!memory) { free(buffer); return;}
      #ifdef webdebug 
        if (size < 300000) {
          UDBDebug("after malloc");
        }
    #endif
  
  thedisplay->Clear();

  uint32_t starttime = millis();

  unsigned long end_ms;
  int framecounter=0;

  while(myfile) {  
    #ifdef webdebug 
        if (size < 300000) {
          Serial.println("frame: "+String(framecounter));
          UDBDebug("frame: "+String(framecounter));
        }
    #endif
    framecounter++;
    if (settings->needRefresh()) {
      settings->doRefresh();
        #ifdef webdebug 
          Serial.println("break refresh");
          UDBDebug("break refresh");
        #endif
      break; 
    }  

    end_ms = millis()+35;  // play a little bit slower as real
    int nextread = myfile.read(buffer, rawsize);
    if (nextread < rawsize)  break;

    uint16_t color, counter=0;
    for (int yy=0; yy<32; yy++) 
      for (int xx=0; xx<128; xx++) {
        if (filetype == 2) 
          color = thedisplay->color565( buffer[counter], buffer[counter+1], buffer[counter+2]);
        else 
          color = thedisplay->color565( buffer[counter+2], buffer[counter+1], buffer[counter]);
          
        if(memory[yy*128+xx] != color ) {
              #ifdef UseDMD
                thedisplay->DrawPixel(xx, yy, buffer[counter], buffer[counter+2], buffer[counter+1]);
              #else
                #ifndef TFT_PARALLEL_8_BIT
                  thedisplay->DrawPixel(xx, yy, color);
                #endif  
              #endif
              memory[yy*128+xx]=color;
          }
       
        counter += 3;
      }
    #ifdef TFT_PARALLEL_8_BIT
     thedisplay->DrawImage(memory);
    #endif   

      #ifdef xxxwebdebug 
        if (size < 500000) {
          UDBDebug("after frame, ms "+String(millis())+" endms "+String(end_ms));
         }
       #endif

    while (millis()<end_ms)     {
      #ifdef xxxwebdebug 
        if (size < 500000) {
          UDBDebug("loop");
         }
       #endif
      //Serial.println("wait");
      loopalwaysrun();
    }

  }
     uint32_t finalendtime = millis();

      #ifdef webdebug 
        if (size < 500000) {
          UDBDebug("before close, after loop");
         }
       #endif  
    myfile.close();
          #ifdef webdebug 
        if (size < 500000) {
          UDBDebug("after close, after loop");
         }
       #endif  
    if (buffer) free(buffer);
          #ifdef webdebug 
        if (size < 500000) {
          UDBDebug("after free buffer");
         }
       #endif  
    if(memory) free(memory);


#ifdef webdebug    
  Serial.println("Video run time: "+String(finalendtime-starttime)+"ms, frames: "+String(framecounter)+" fps: "+ String(framecounter*1000/((finalendtime-starttime))));
  UDBDebug("Video run time: "+String(finalendtime-starttime)+"ms, frames: "+String(framecounter)+" fps: "+ String(framecounter*1000/((finalendtime-starttime))));
#endif


  end_ms = millis()+250;  // wait half a second
  while (millis()<end_ms) 
    loopalwaysrun();  
}

void getFilesList(File dir) {
  noFiles = getVideoFiles(dir,0);
}  

void getFontList(File dir) {
  noFonts = getFontFiles(dir,0);
}  

short getCacheList(String path) {
  File card;
  String filename;
  short filetype=0;

  noFiles=0;

  card = SD.open(path);
  if(card) {
    while(card.available()) {
      filename = card.readStringUntil('\n');
      if ((filename.endsWith("\r"))) filename.remove(filename.length()-1);
      // remove \r
      filetype=0;
      if ((filename.endsWith(".mjpeg"))) filetype=1;
      if ((filename.endsWith(".rgb"))) filetype=2;
      if ((filename.endsWith(".bgr"))) filetype=3;

      filetypes[noFiles] = filetype;
      filenames[noFiles++] = filename;

    }
    card.close();
  }
  return noFiles;
}

short getFontCache(String path) {
  File card;
  String filename;

  noFonts=0;

  card = SD.open(path);
  if(card) {
    while(card.available()) {
      filename = card.readStringUntil('\n');
      if ((filename.endsWith("\r"))) filename.remove(filename.length()-1); // remove \r
      fontnames[noFonts++] = filename;
    }
    card.close();
  }
  return noFonts;
}

void playRandomVideo() {
  short next = random(noFiles);
  
  while(nextclipid == next) {
    next = random(noFiles);
  }
  
  nextclipid = next;

  String name = filenames[nextclipid];
  short filetype = filetypes[nextclipid];

  if (filetype == 1)
    PlayVideo( name);
  else
    PlayRawVideo(name, filetype);
}
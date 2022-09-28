#include "digits.h"
#include "SD.h"


#ifdef UseDMD
    Digits::Digits( MatrixPanel_I2S_DMA *display);
            Digits::display = display;
#else
    Digits::Digits(Arduino_GFX *display){
        Digits::display = display;
    }
#endif   


void Digits::SetFont(String name) {
    Digits::fontname = name;
    if (Digits::fontbuffer) free(Digits::fontbuffer);

    if (!SD.exists(name)) return;
    File root;
    root = SD.open(name, "r", false);
    int16_t wordbuffer;
    Digits::count = root.read(); //1 byte;
    Digits::height = root.read(); 

    if (Digits::count > 11)
        Digits::count = 11;

    // now for count 1 byte width, 2 byte offset in buffer
    for (int i=0; i<Digits::count; i++) {
        Digits::charwidths[i] = root.read(); 
        wordbuffer =  root.read() << 8;
        wordbuffer += root.read(); 
        Digits::charoffset[i] = wordbuffer;     
    }

    wordbuffer = root.available();
      Digits::fontbuffer = (char *) malloc(wordbuffer);  
    wordbuffer=root.read((uint8_t *) Digits::fontbuffer, wordbuffer);
    Digits::fontsize = wordbuffer;
    root.close();
}

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

// x=left, y = top
int8_t Digits::DrawChar(char thechar, int8_t x, int8_t y, int16_t color) {
    if (y < 0)
        y = ((32-Digits::height)/2);

    int8_t digit = thechar-48;
    if ((digit<0)|(digit>9))
        digit = 10;

    int8_t width = Digits::charwidths[digit];
    int8_t width_round = ((width/8)+1)*8;  
    int16_t offset = Digits::charoffset[digit];
    uint8_t byte = 0, pos=0;
    int8_t bit = 7;
    int16_t maxsize = Digits::fontsize; 

    for (int8_t yy = 0; yy < Digits::height; yy++) {
        bit = 7;
        byte = Digits::fontbuffer[offset+pos];      

        for (int8_t xx = 0; xx < width_round; xx++) {
            if (CHECK_BIT(byte, bit)) {
                display->drawRect((xx+x)*2, (yy+y)*2, 2, 2, color);
            }
            bit--;
            if (bit<0) {
                bit = 7;
                if ((pos+offset+1) < maxsize) {
                    pos++;
                    byte = Digits::fontbuffer[pos + offset];
                }
            }
        }
    }

    return width;
}

 void Digits::DrawString(String text, int8_t x, int8_t y, int16_t color) {
     int8_t pos=x;
     int8_t width;

    for (int i=0; i<text.length(); i++) {
        width = Digits::DrawChar(text.charAt(i), pos, y, color);
        pos += (width+2);
    }
 }

 void Digits::DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec) {

 }

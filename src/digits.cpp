#include "digits.h"
#include "SD.h"
#include "video.h"


Digits::Digits(Display *thedisplay){
       display = thedisplay;
    }

extern u_int16_t frameColor;
extern String fontnames[];
extern bool displaySeconds;
extern bool twelveHourFormat;

void Digits::SetFont(String name) {
    fontname = name;
    if (fontbuffer) free(fontbuffer);

    if (!SD.exists(name)) return;
    File root;
    root = SD.open(name, "r", false);
    int16_t wordbuffer;
    count = root.read(); //1 byte;
    height = root.read(); 

    if (count > 11)
        count = 11;

    // now for count 1 byte width, 2 byte offset in buffer
    for (int i=0; i<count; i++) {
        charwidths[i] = root.read(); 
        wordbuffer =  root.read() << 8;
        wordbuffer += root.read(); 
        charoffset[i] = wordbuffer;     
    }

    wordbuffer = root.available();
      fontbuffer = (char *) malloc(wordbuffer);  
    wordbuffer=root.read((uint8_t *) fontbuffer, wordbuffer);
    fontsize = wordbuffer;
    root.close();
}

void Digits::SetFontNumber(int8_t number) {
    if (number == 0)
        fontnumber = 0;
    else    
        fontnumber = number-1;
    SetFont(fontnames[number]);
}

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

// x=left, y = top
int8_t Digits::DrawDigit(int8_t digit, int8_t x, int8_t y, int16_t color) {    
    if (y < 0)
        y = ((32-height)/2);

    if ((digit < 0) || (digit>10)) return 0;  

    int8_t width = charwidths[digit];

    int8_t width_round = ((width/8)+1)*8;  
    int16_t offset = charoffset[digit];
    uint8_t byte = 0, pos=0;
    int8_t bit = 7;
    int16_t maxsize = fontsize; 

    for (int8_t yy = 0; yy < height; yy++) {
        bit = 7;
        byte = fontbuffer[offset+pos];      
        for (int8_t xx = 0; xx < width_round; xx++) {
            if (CHECK_BIT(byte, bit)) {
                display->DrawPixel(xx+x, yy+y, color);   
            }
            bit--;
            if (bit<0) {
                bit = 7;
                if ((pos+offset+1) < maxsize) {
                    pos++;
                    byte = fontbuffer[pos + offset];                   
                }
            }
        }
    }

    return width;
}

// x=left, y = top
int8_t Digits::DrawChar(char thechar, int8_t x, int8_t y, int16_t color) {

    int8_t digit = thechar-48;
    if ((digit<0)|(digit>9))
        digit = 10;

    return DrawDigit(digit, x, y, color);
}

void Digits::DrawString(String text, int8_t x, int8_t y, int16_t color) {
     int8_t pos=x;
     int8_t width;

    for (int i=0; i<text.length(); i++) {
        width = DrawChar(text.charAt(i), pos, y, color);
        pos += (width+2);
    }
}

int8_t Digits::CalcDigitWidth(int8_t digit) {
    if ((digit<0)|(digit>9))
        digit = 10;
    return charwidths[digit];
}

int16_t Digits::CalcTimeWidth( int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int8_t distance) {
    int16_t width = 0;

    if (cur_hour>9)
        width = CalcDigitWidth(cur_hour/10) + distance;
    width += CalcDigitWidth(cur_hour%10) + distance;
    width += charwidths[10] + distance;
    width += CalcDigitWidth(cur_min/10) + distance;
    width += CalcDigitWidth(cur_min%10) + distance; 
    if (displaySeconds) {
        width += charwidths[10] + distance;
        width += CalcDigitWidth(cur_sec/10) + distance;
        width += CalcDigitWidth(cur_sec%10) + distance;        
    }
    return width;
}

 void Digits::DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int16_t& timeCounter) {
  static int16_t last_hour = -1;
  static int16_t last_min  = -1;
  static int16_t last_sec  = -1;  
  int8_t y = -1, width=0, distance=2;

  if (twelveHourFormat)
    if (cur_hour>12)
        cur_hour-=12;

  int8_t x = 64 - (CalcTimeWidth( cur_hour, cur_min, cur_sec, distance) / 2);
  if (x<0) x = 1;

  if ((cur_hour != last_hour) | (cur_min != last_min) | (timeCounter == 0)) {
    timeCounter++;
    display->FillRect(0, 0, 128, 32, BLACK);
    display->DrawRect(0, 0, 128, 32, frameColor);

    if (cur_hour>9)
        width = DrawDigit(cur_hour/10, x, y, fontcolor) + distance;
    width += DrawDigit(cur_hour%10, x+width, y, fontcolor) + distance;
    lastcolon = x+width;
    if ((cur_sec % 2) == 1)
      width += DrawDigit(10, x+width, y, fontcolor) + distance;
    else 
      width += charwidths[10] + distance;
    width += DrawDigit(cur_min/10, x+width, y, fontcolor) + distance;
    width += DrawDigit(cur_min%10, x+width, y, fontcolor) + distance; 
    if (displaySeconds) {
        lastseccolon = x+width;
        if ((cur_sec % 2) == 1)
            width += DrawDigit(10, x+width, y, fontcolor) + distance;
        else 
        width += charwidths[10] + distance;
        width += DrawDigit(cur_sec/10, x+width, y, fontcolor) + distance;
        width += DrawDigit(cur_sec%10, x+width, y, fontcolor) + distance; 
    }

  }
  else
  if(cur_sec != last_sec) {
    timeCounter++;
    if ((cur_sec % 2) == 1) {
        DrawDigit(10, lastcolon, y, fontcolor);
        if (displaySeconds)
            DrawDigit(10, lastseccolon, y, fontcolor);
    } 
    else 
    {
      DrawDigit(10, lastcolon, y, BLACK);
      if (displaySeconds)
        DrawDigit(10, lastseccolon, y, BLACK);
    }
  }


  last_hour = cur_hour;
  last_min = cur_min;
  last_sec = cur_sec;

 }

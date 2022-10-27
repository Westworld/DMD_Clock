#include "digits.h"
#include "SD.h"
#include "video.h"

extern String fontnames[];

const int sparkleRandom=3;

Digits::Digits(Display *thedisplay, Settings *thesettings){
       display = thedisplay;
       settings = thesettings;
    }

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

void Digits::CheckFont(void) {
    int8_t font = settings->getFontNumber();
    if (fontnumber != font) {
        SetFontNumber(font);
    }
}

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


int8_t Digits::DrawDigit(int8_t digit, int8_t x, int8_t y, uint16_t color) {    
    if (y < 0)
        y = ((32-height)/2);

    if ((digit < 0) || (digit>10)) return 0;  

    int8_t width = charwidths[digit];

    int8_t width_round = ((width/8)+1)*8;  
    int16_t offset = charoffset[digit];
    uint8_t byte = 0, pos=0;
    int8_t bit = 7;
    int16_t maxsize = fontsize; 
    uint16_t thecolor;

    int8_t theheight = height;
    if (upDownCounter > 0) 
        if (upDownCounter < height)
            theheight = upDownCounter;

    bool doSparkle = settings->getClockSparkle();  
    uint16_t SparkleColor = settings->getFontSparkleColor();

    for (int8_t yy = 0; yy < theheight; yy++) {
        bit = 7;
        byte = fontbuffer[offset+pos];      
        for (int8_t xx = 0; xx < width_round; xx++) {
            if (CHECK_BIT(byte, bit)) {
                thecolor = color;
                if ((color != BLACK) & (doSparkle)) {
                    int8_t ran = random(sparkleRandom);  // 0-(sparkleRandom-1)
                    if (ran == 0) thecolor=SparkleColor; 
                }
                display->DrawPixel(xx+x, yy+y, thecolor);   
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

int8_t Digits::DrawDigitCheck(int8_t digit, int8_t x, int8_t y, int8_t type ) {
    switch (type) {
        case -1: 
            return DrawDigit(digit, x, y, BLACK);
            break;
        case 1: 
            return DrawDigit(digit, x, y, settings->getFontColor());
            break;
        default:
            return CalcDigitWidth(digit);        
    }
}

int8_t Digits::DrawChar(char thechar, int8_t x, int8_t y, uint16_t color) {

    int8_t digit = thechar-48;
    if ((digit<0)|(digit>9))
        digit = 10;

    return DrawDigit(digit, x, y, color);
}

void Digits::DrawString(String text, int8_t x, int8_t y, uint16_t color) {
     int8_t pos=x;
     int8_t width;

     CheckFont();

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
    if (settings->getDisplaySeconds()) {
        width += charwidths[10] + distance;
        width += CalcDigitWidth(cur_sec/10) + distance;
        width += CalcDigitWidth(cur_sec%10) + distance;        
    }
    return width;
}


int8_t Digits::DrawTime (int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int8_t draw1, int8_t draw2, int8_t draw3, int8_t draw4, int8_t draw5, int8_t draw6, int8_t draw7, int8_t draw8) {
     int8_t y = -1, width=0, distance=2;
     uint16_t fontcolor = settings->getFontColor();

   if (cur_hour>9)
        width = DrawDigitCheck(cur_hour/10, start_x, y, draw1) + distance;
    width += DrawDigitCheck(cur_hour%10, start_x+width, y, draw2) + distance;
    lastcolon = start_x+width;
    width += DrawDigitCheck(10, start_x+width, y, draw3) + distance;
    width += DrawDigitCheck(cur_min/10, start_x+width, y, draw4) + distance;
    width += DrawDigitCheck(cur_min%10, start_x+width, y, draw5) + distance; 
    if (settings->getDisplaySeconds()) {
        lastseccolon = start_x+width;
        width += DrawDigitCheck(10, start_x+width, y, draw6) + distance;
        width += DrawDigitCheck(cur_sec/10, start_x+width, y, draw7) + distance;
        width += DrawDigitCheck(cur_sec%10, start_x+width, y, draw8) + distance; 
    }   
    return width;  
 }

 void Digits::DrawTime(  int16_t cur_hour, int16_t cur_min, int16_t cur_sec, int16_t& timeCounter) {
  int8_t y = -1, width=0, distance=2;

  CheckFont();
  uint16_t fontcolor = settings->getFontColor();

  if (settings->getTwelveHourFormat())
    if (cur_hour>12)
        cur_hour-=12;

  if ((upDownCounter>0) & (!settings->getClockUpDown())) // reset
      upDownCounter = 0;

  int8_t draw1=0, draw2=0,  draw3=0,  draw4=0,  draw5=0,  draw6=0,  draw7=0,  draw8=0;

  if (upDownCounter > 30) upDownCounter = 0;

  // wenn min/hour update, needrefresh oder updowncounter, alles malen, vor her löschen
  // wenn sekunden update und display seconds, erst black, dann white malen
  // sonst dots blinken
  // nichts - sparkle#######

  if ( (cur_hour != last_hour) | (cur_min != last_min) | (settings->needRefresh()) ) 
  { if (upDownCounter == 0)
    {   timeCounter++;
        display->FillRect(0, 0, 128, 32, BLACK);
        display->DrawRect(0, 0, 128, 32, settings->getFrameColor());
        start_x = 64 - (CalcTimeWidth( cur_hour, cur_min, cur_sec, distance) / 2);
        if (start_x<0) {
            start_x = 1;
        }
        if (settings->getClockUpDown())
            upDownCounter = 1; 
    }       

    if (cur_hour>9)
        draw1=1;
    draw2=1;
    lastcolon = start_x+width;
    if ((cur_sec % 2) == 1)
        draw3=1;
    else 
        draw3=-1;
    draw4=1;
    draw5=1; 
    if (settings->getDisplaySeconds()) {
        lastseccolon = start_x+width;
        if ((cur_sec % 2) == 1)
            draw6=1;
        else 
            draw6=-1;
        draw7=1;
        draw8=1;   
        timeCounter++;  
    }    
    DrawTime (cur_hour, cur_min, cur_sec, draw1, draw2, draw3, draw4, draw5, draw6, draw7, draw8);
  }
  else
    if ((settings->getDisplaySeconds() & (cur_sec != last_sec)))  {
        timeCounter++;
        draw1=draw2=draw4=draw5=0;
        draw3=draw6=draw7=draw8=-1;
        DrawTime (cur_hour, cur_min, last_sec, draw1, draw2, draw3, draw4, draw5, draw6, draw7, draw8);
        if ((cur_sec % 2) == 1)
            draw3=draw6=1;
        else 
            draw3=draw6=-1;   
        draw7=draw8=1;  
        DrawTime (cur_hour, cur_min, cur_sec, draw1, draw2, draw3, draw4, draw5, draw6, draw7, draw8);      
    }
    else 
    if(cur_sec != last_sec) {
        timeCounter++;
        if (settings->getDisplaySeconds()) {
            // we already did everything...
        }
        else {
            draw1=0, draw2=0,  draw3=0,  draw4=0,  draw5=0,  draw6=0,  draw7=0,  draw8=0;
            if ((cur_sec % 2) == 1) {
                draw3=1;
                if (settings->getDisplaySeconds())
                   draw6=1;
            } 
            else 
                {
                draw3=-1;
                if (settings->getDisplaySeconds())
                    draw6=-1;
                }
            DrawTime (cur_hour, cur_min, cur_sec, draw1, draw2, draw3, draw4, draw5, draw6, draw7, draw8);
        }
    }  
    else {
        // anything else, only display if sparkle, run this once per loops (delay??)
        if ((settings->getClockSparkle())| (upDownCounter>0))  {
            draw1=1, draw2=1,  draw3=0,  draw4=1,  draw5=1,  draw6=0,  draw7=0,  draw8=0;
            if ((cur_sec % 2) == 1) {
                draw3=1;
                if (settings->getDisplaySeconds())
                   draw6=1;
            } 
            if (settings->getDisplaySeconds())
                draw7=draw8=1;
            DrawTime (cur_hour, cur_min, cur_sec, draw1, draw2, draw3, draw4, draw5, draw6, draw7, draw8);  
            if (upDownCounter>0)
                upDownCounter++;
            else    
                delay(10);    
        }
    }      
    


  last_hour = cur_hour;
  last_min = cur_min;
  last_sec = cur_sec;

 }


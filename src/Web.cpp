#include "Web.h"

extern int16_t displayTime;

#define webdebug 1


void Web_timedisplayCall(Control* sender, int type)
{
  #ifdef webdebug
    Serial.print("Time Display: ");
    Serial.println(sender->value);
  #endif

  displayTime = sender->value.toInt();
}

void Web_Init() {
  ESPUI.number("Time display in seconds:", &Web_timedisplayCall, ControlColor::Alizarin, displayTime);
  ESPUI.begin("DMD Clock");
}
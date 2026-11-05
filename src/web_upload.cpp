#include <Arduino.h>
#include "SD.h"
#include "web_upload.h"
#include <ESPAsyncWebServer.h>
#include "web_upload_html.h"

// 2nd web server
AsyncWebServer *server;

void Web_Upload_Init() {
    server = new AsyncWebServer(81);
    server->on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
    {
        #ifdef webdebug 
            String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
            Serial.println(logmessage);
        #endif
        request->send(200, "text/plain", listFiles(true));
    });
}

// list all of the files, if ishtml=true, return html rather than simple text
String listFiles(bool ishtml) {
  String returnText = "";
  #ifdef webdebug 
  Serial.println("Listing files stored on sdcard");
  #endif
  File root = SD.open("/clips");
  File foundfile = root.openNextFile();
  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
  }
  while (foundfile) {
    if (ishtml) {
      returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'download\')\">Download</button>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";
    } else {
      returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
    }
    foundfile = root.openNextFile();
  }
  if (ishtml) {
    returnText += "</table>";
  }
  root.close();
  foundfile.close();
  return returnText;
}

// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
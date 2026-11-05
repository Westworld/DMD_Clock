#ifndef web_upload_h
#define web_upload_h 

void Web_Upload_Init();
String humanReadableSize(const size_t bytes);
String listFiles(bool ishtml);
#endif
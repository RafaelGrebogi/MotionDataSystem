#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <ArduinoJson.h>

extern char chipIDChar[16];

String prepareJsonPayload();
char* ESP32_ID_Extraction();

void collectData(float ax, float ay, float az, float gx, float gy, float gz, String time);

#endif  // JSON_HANDLER_H
#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <ArduinoJson.h>
#include "time.h"

extern char chipIDChar[16];

// Define the number of samples per batch
#define BATCH_SIZE 10
extern int dataCount;

String getCurrentTimestamp();
String prepareJsonPayload();
char* ESP32_ID_Extraction();
void sendJsonToPC();
String generateMessageID();
String getMsgTimestamp();

void collectData(float ax, float ay, float az, float gx, float gy, float gz, String time);

#endif  // JSON_HANDLER_H
#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H


#include <ArduinoJson.h>
#include "time.h"
#include "http_handler.h"

extern char chipIDChar[16];
extern char msgID[50];

// Define the number of samples per batch
#define BATCH_SIZE 10

extern int dataCount;
extern int msgCounter;

String getCurrentTimestamp();
String prepareJsonPayload();
char* ESP32_ID_Extraction();
void sendJsonToPC();
String generateMessageID();
String getMsgTimestamp();

void collectData(String targetLabel, float ax, float ay, float az, float gx, float gy, float gz, String time);

#endif  // JSON_HANDLER_H
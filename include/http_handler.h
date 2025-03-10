#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "api_credentials.h"



void sendDataToThingSpeak(float value);

#endif  // HTTP_HANDLER_H
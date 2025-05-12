#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "wifi_credentials.h"  

extern WiFiClient client;

void ConnectToWifi();
bool connect2network(const char* ssid, const char* password);

#endif  // WIFI_HANDLER_H
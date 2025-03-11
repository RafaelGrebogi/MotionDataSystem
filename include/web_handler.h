#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

extern WebServer server;  // Declare the web server globally
extern bool isAcquiring;

void StartWebServer();  // Function to initialise the web server
void handleStart();
void handleStop();
void handleStatus();

#endif  // WEB_HANDLER_H

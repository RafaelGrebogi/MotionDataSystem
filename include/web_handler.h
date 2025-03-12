#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

extern WebServer server;  // Declare the web server globally
extern bool isAcquiring;
extern hw_timer_t *My_timer;

void StartWebServer();  // Function to initialise the web server
void handleStart();
void handleStop();
void handleStatus();

void test_handleStart();
void test_handleStop();

#endif  // WEB_HANDLER_H

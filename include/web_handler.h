#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
// #include "http_handler.h"
// #include "json_handler.h"

extern WebServer server;  // Declare the web server globally
extern bool isAcquiring;
extern hw_timer_t *My_timer;
extern String targetLabel;  // Holds the selected training label

// Functions for main data acquisition web server
void StartWebServer();  // Function to initialise the web server
void handleStart();
void handleStop();
void handleStatus();

// Functions for training data acquisition web server
void StartWebServerTRAIN();
void handleTrainingRoot();
void handleTrainStart();
void handleTrainStop();
void handleTrainStatus();
void handleTrainInfo();

extern void safeTriggerFastAPI();

// Functions for web server Unit Test
void test_handleStart();
void test_handleStop();

#endif  // WEB_HANDLER_H

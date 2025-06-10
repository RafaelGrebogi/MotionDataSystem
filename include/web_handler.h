#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "calibration_handler.h"
#include "config.h"
// #include "http_handler.h"
// #include "json_handler.h"



extern WebServer server;  // Declare the web server globally
extern bool isAcquiring;
extern hw_timer_t *My_timer;
extern String targetLabel;  // Holds the selected training label
extern bool TRAINING_MODE;
extern bool TESTING_MODE;


void InitialWebServerSetup();

void handleProductionRoot();
void handleRoot();

void handleLogin();
void handleLoginSubmit();
void handleLogout();

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
void handleLabelUpdate();

void handleCalibration();

extern void safeTriggerFastAPI();
extern UserStatus safeFetchUserId(String username);

// Functions for web server Unit Test
void test_handleStart();
void test_handleStop();

#endif  // WEB_HANDLER_H

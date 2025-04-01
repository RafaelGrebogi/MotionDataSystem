#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "time.h"
#include "api_credentials.h"
#include "json_handler.h"

#include <Firebase_ESP_Client.h>

#define FIREBASE_CONTROL_PATH "/ControlFlag/"
#define FIREBASE_TRAINING_PATH "/ESP32_Develop/TrainingDataset/"
#define FIREBASE_PRODUCTION_PATH "/ESP32_Develop/Data/"

extern FirebaseData fbdo;  // Firebase data object
extern FirebaseAuth auth;
extern FirebaseConfig config;

// int sendDataToThingSpeak(float value);

void initiliaseFirebase();
void sendDataToFirebase(String jsonData);
void sendCompleteFlag();
void debugInternetConnection();

#endif  // HTTP_HANDLER_H
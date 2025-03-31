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

#define FASTAPI_IP_TRIGGER "http://192.168.20.10:8000/trigger-training"



extern FirebaseData fbdo;  // Firebase data object
extern FirebaseAuth auth;
extern FirebaseConfig config;

// int sendDataToThingSpeak(float value);

void initiliaseFirebase();
void sendDataToFirebase(String jsonData);
void sendCompleteFlag();
void triggerFastAPI();
void startFastAPIConnection();
void stopFastAPIConnection();
void debugInternetConnection();

#endif  // HTTP_HANDLER_H
#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "time.h"
#include "api_credentials.h"
#include "json_handler.h"
#include "config.h"

#include <Firebase_ESP_Client.h>

#define FIREBASE_DEVELOP_CONTROL_PATH "/ESP32_Develop/ControlFlag/"
#define FIREBASE_TRAINING_PATH "/ESP32_Develop/TrainingDataset/"
#define FIREBASE_TESTING_PATH "/ESP32_Develop/TestingDataset/"

#define FIREBASE_PRODUCTION_PATH "/ESP32_Production/Data/"
#define FIREBASE_PRODUCTION_CONTROL_PATH "/ESP32_Production/ControlFlag/"

// #define FASTAPI_IP_TRIGGER "http://192.168.20.5:8000/"




// Global variable to store the FastAPI IP
extern String fastapi_ip; 

extern FirebaseData fbdo;  // Firebase data object
extern FirebaseAuth auth;
extern FirebaseConfig config;

// int sendDataToThingSpeak(float value);

void initiliaseFirebase();
void sendDataToFirebase(String jsonData);
void sendCompleteFlag();
void triggerFastAPI();
bool fetchFastApiIP();

UserStatus fetchUserStatusFromAPI(String username, String device_id);
UserStatus fetchServiceStatusFromAPI(String serviceId);

void debugInternetConnection();

#endif  // HTTP_HANDLER_H
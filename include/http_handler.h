#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "api_credentials.h"
#include "json_handler.h"

#include <Firebase_ESP_Client.h>
// #include <json/FirebaseJson.h>

// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"


extern FirebaseData fbdo;  // Firebase data object
extern FirebaseAuth auth;
extern FirebaseConfig config;

// int sendDataToThingSpeak(float value);

void initiliaseFirebase();
void sendDataToFirebase(String jsonData);

void debugInternetConnection();

#endif  // HTTP_HANDLER_H
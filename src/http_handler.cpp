#include "http_handler.h"
#include "wifi_handler.h"
#include "config.h"  // Include global configuration

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


// WiFiClient client;

String fastapi_ip = "192.168.20.5";  // Default/fallback IP  = "192.168.20.5"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

OperationMode currentMode;

char msgID[50];
// char chipIDChar;

//#################################################################
void initiliaseFirebase(){

  Serial.println("🔄 Initializing Firebase with OAuth2.0...");

  // Assign the database URL
  config.database_url = FirebaseBaseUrl;

  // Assign service account credentials
  config.service_account.data.project_id = FirebaseProject_id;
  config.service_account.data.client_email = FirebaseClient_email;
  config.service_account.data.private_key = PRIVATE_KEY_STR;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);



  debugInternetConnection();

  if (Firebase.ready()) {
    Serial.println("✅ Firebase successfully initialized.");
} else {
    Serial.println("❌ Firebase initialization failed.");
}


}

//#################################################################
void sendDataToFirebase(String jsonData) {
  FirebaseJson json;
  json.setJsonData(jsonData);  // Convert String to FirebaseJson

  char databasePath[80];
  if (currentMode == TRAINING) {
    // Each dataset entry will include a target label (e.g., "Normal Walk" or "Limping")
    sprintf(databasePath, "%s%s",FIREBASE_TRAINING_PATH,msgID); 
  } else if (currentMode == TESTING) {
    // Each dataset entry will include a target label (e.g., "Normal Walk" or "Limping")
    sprintf(databasePath, "%s%s",FIREBASE_TESTING_PATH,msgID); 
  } else if (currentMode == PRODUCTION) {
    // No target labels will be included in this mode
    sprintf(databasePath, "%s%s",FIREBASE_PRODUCTION_PATH,msgID);  // Create dataset with no target
  }
  // if(TRAINING_MODE){
  // // Each dataset entry will include a target label (e.g., "Normal Walk" or "Limping")
  //   sprintf(databasePath, "%s%s",FIREBASE_TRAINING_PATH,msgID);  
  // }else{
  // // No target labels will be included in this mode
  //   sprintf(databasePath, "%s%s",FIREBASE_PRODUCTION_PATH,msgID);  // Create dataset with no target
  // }



  if (Firebase.ready()) {
      if (!Firebase.RTDB.setJSON(&fbdo, String(databasePath), &json)) {
        Serial.println(fbdo.errorReason());
      } 
  }
  
}

//#################################################################
void sendCompleteFlag() {
  FirebaseJson json;
  json.set("complete", true); 
  json.set("timestamp", getCurrentTimestamp());
  json.set("device_id",chipIDChar);

  // Build control path
  char controlPath[64];
  if (currentMode == TRAINING) {
    snprintf(controlPath, sizeof(controlPath), "%s%s", FIREBASE_DEVELOP_CONTROL_PATH, chipIDChar);    // DEVELOPMENT MODE
  } else if (currentMode == TESTING) {
    snprintf(controlPath, sizeof(controlPath), "%s%s", FIREBASE_DEVELOP_CONTROL_PATH, chipIDChar);    // DEVELOPMENT MODE
  } else if (currentMode == PRODUCTION) {
    snprintf(controlPath, sizeof(controlPath), "%s%s", FIREBASE_PRODUCTION_CONTROL_PATH, chipIDChar); // PRODUCTION MODE
  }


  if (!Firebase.RTDB.setJSON(&fbdo, controlPath, &json)) {
    Serial.println(fbdo.errorReason());
  }

}



//#################################################################
void triggerFastAPI() {
  HTTPClient http;

  // String url = FASTAPI_IP_TRIGGER;
  char url[100];  // Adjust size as needed
  snprintf(url, sizeof(url), "http://%s:8000/", fastapi_ip.c_str());


  if (currentMode == TRAINING) {
    snprintf(url + strlen(url), sizeof(url) - strlen(url), "trigger-training");
  } else if (currentMode == TESTING) {
    snprintf(url + strlen(url), sizeof(url) - strlen(url), "trigger-testing");
  } else if (currentMode == PRODUCTION) {
    snprintf(url + strlen(url), sizeof(url) - strlen(url), "trigger-production");
  }


  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);  // 5sec
  

  char body[100];
  snprintf(body, sizeof(body), "{\"device_id\": \"%s\"}", chipIDChar);
  int httpCode = http.POST(String(body));

  if (httpCode > 0) {
    Serial.printf("✅ Trigger sent to FastAPI: %d\n", httpCode);
  } else {
    Serial.printf("❌ Failed to trigger FastAPI: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}



//#################################################################
bool fetchFastApiIP() {
  if (Firebase.RTDB.getString(&fbdo, "/ServerIP")) {
      if (fbdo.dataType() == "string") {
          fastapi_ip = fbdo.stringData();
          char message[100];
          snprintf(message, sizeof(message), "✅ Fetched FastAPI IP from Firebase: %s", fastapi_ip.c_str());
          Serial.println(message);
          return true;
      } else {
          Serial.println("⚠️ Unexpected data type for FastAPI IP.");
          return false;
      }
  } else {
      char message[100];
      snprintf(message, sizeof(message), "❌ Failed to get FastAPI IP: %s", fbdo.errorReason());
      Serial.println(message);
      return false;
  }
}

//#################################################################
void debugInternetConnection(){
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi is connected. Testing Internet access...");
    // WiFiClient client;
    if (!client.connect("www.google.com", 80)) {
        Serial.println("❌ Internet access FAILED! Check network connection.");
    } else {
        Serial.println("✅ Internet is working.");
        client.stop();
    }
  } else {
    Serial.println("❌ WiFi is NOT connected.");
}



}


// int sendDataToThingSpeak(float value) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         String url = String(thingSpeakBaseUrl) + "?api_key=" + String(thingSpeakApiKey) + "&field1=" + String(value);
//         http.begin(client, url);
//         int httpResponseCode = http.GET();

//         if (httpResponseCode>0) {
//             Serial.print("ThingSpeak Response: ");
//             Serial.println(httpResponseCode);
//           }
//           else {
//             Serial.print("Error code: ");
//             Serial.println(httpResponseCode);
//           }
//           String s = http.getString();
//           Serial.print(s + "\n");
        
//         http.end();
//         return httpResponseCode;
//     } else {
//         Serial.println("WiFi Disconnected");
//         return -1;
//     }
// }



//#### Firebase debugging (if necessary)
//     Serial.println("❌ Firebase initialization failed. Debugging...");
//     // Print authentication errors
//     if (config.signer.tokens.status == token_status_error) {
//         Serial.print("Firebase Auth Error: ");
//         Serial.println(config.signer.tokens.error.message.c_str());
//     }
// }
#include "http_handler.h"
#include "wifi_handler.h"
#include "config.h"  // Include global configuration

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


// WiFiClient client;


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

char msgID[50];

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
//     Serial.println("❌ Firebase initialization failed. Debugging...");
//     // Print authentication errors
//     if (config.signer.tokens.status == token_status_error) {
//         Serial.print("Firebase Auth Error: ");
//         Serial.println(config.signer.tokens.error.message.c_str());
//     }

//     // Check if WiFi is connected
//     if (WiFi.status() != WL_CONNECTED) {
//         Serial.println("❌ WiFi is NOT connected. Firebase cannot initialize.");
//     } else {
//         Serial.println("✅ WiFi is connected, but Firebase is not ready.");
//     }
// }


}

//#################################################################
void sendDataToFirebase(String jsonData) {
  FirebaseJson json;
  json.setJsonData(jsonData);  // Convert String to FirebaseJson

  char databasePath[80];
  #ifdef TRAINING_MODE
  // Each dataset entry will include a target label (e.g., "Normal Walk" or "Limping")
    sprintf(databasePath, "%s%s","/ESP32_Develop/TrainingDataset/",msgID);  
  #else
  // No target labels will be included in this mode
    sprintf(databasePath, "%s%s","/ESP32_Develop/Data/",msgID);  // Create dataset with no target
  #endif
//   sprintf(databasePath, "%s%s","/ESP32_Develop/Data/",msgID);


  if (Firebase.ready()) {
      if (Firebase.RTDB.setJSON(&fbdo, String(databasePath), &json)) {
          // Serial.println(" Data sent to Firebase!");
      } else {
          Serial.print(" Firebase Error: ");
          Serial.println(fbdo.errorReason());
      }
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
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

//#################################################################


UserStatus fetchServiceStatusFromAPI(String serviceId) {
  HTTPClient http;
  char apiUrl[200];

  snprintf(apiUrl, sizeof(apiUrl),
           "http://%s:8000/get-service-status?service_id=%s",
           fastapi_ip.c_str(), serviceId.c_str());

  http.setTimeout(5000);
  http.begin(apiUrl);
  int httpCode = http.GET();

  UserStatus result;
  result.success = false;
  result.userId = "";
  result.hasActiveService = false;
  result.selectedServiceId = serviceId;
  result.serviceCount = 0;

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Received from FastAPI:");
    Serial.println(payload);

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print("JSON parsing error: ");
      Serial.println(String(error.c_str()));
      http.end();
      return result;
    }

    // Required fields
    result.userId = doc["user_id"].as<String>();
    result.hasActiveService = doc["has_active_service"];
    result.selectedServiceId = doc["service_id"].as<String>();

    // Optional single service info
    JsonObject company = doc["company"];
    if (!company.isNull()) {
      result.services[0].id = serviceId;
      result.services[0].companyName = company["name"].as<String>();
      result.serviceCount = 1;
    }

    result.success = true;

  } else {
    Serial.printf("Error calling FastAPI, code: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return result;
}




//#################################################################

//#################################################################



UserStatus fetchUserStatusFromAPI(String username, String device_id) {
  HTTPClient http;
  char apiUrl[200];

  snprintf(apiUrl, sizeof(apiUrl),
           "http://%s:8000/get-user-status?username=%s&device_id=%s",
           fastapi_ip.c_str(), username.c_str(), device_id.c_str());

  http.setTimeout(5000);
  http.begin(apiUrl);
  int httpCode = http.GET();

  UserStatus result;
  result.success = false;
  result.userId = "";
  result.hasActiveService = false;
  result.selectedServiceId = "";
  result.serviceCount = 0;

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Received from FastAPI:");
    Serial.println(payload);

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print("JSON parsing error: ");
      Serial.println( String(error.c_str()));
      http.end();
      return result;
    }

    // Extract fields
    result.userId = doc["user_id"].as<String>();
    result.hasActiveService = doc["has_active_service"];
    result.selectedServiceId = doc["selected_service_id"].as<String>();

    // Extract services array
    JsonArray services = doc["services"].as<JsonArray>();
    int count = 0;
    for (JsonObject s : services) {
      if (count >= MAX_SERVICES) break;
      result.services[count].id = s["id"].as<String>();
      result.services[count].companyName = s["company"]["name"].as<String>();
      count++;
    }
    result.serviceCount = count;
    result.success = true;

  } else {
    Serial.printf("Error calling FastAPI, code: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return result;
}


  
//#################################################################
void triggerFastAPI() {
  HTTPClient http;


  char url[150];  // Make sure this buffer is large enough
  snprintf(url, sizeof(url), "http://%s:8000/", fastapi_ip.c_str());

  // Append correct endpoint based on mode
  if (currentMode == TRAINING) {
    strncat(url, "trigger-training", sizeof(url) - strlen(url) - 1);
  } else if (currentMode == TESTING) {
    strncat(url, "trigger-testing", sizeof(url) - strlen(url) - 1);
  } else if (currentMode == PRODUCTION) {
    strncat(url, "trigger-production", sizeof(url) - strlen(url) - 1);
  }

  // // Append UserId query parameter
  // strncat(url, "?UserId=", sizeof(url) - strlen(url) - 1);
  // strncat(url, status.userId.c_str(), sizeof(url) - strlen(url) - 1);

  // // Append ServiceId query parameter
  // strncat(url, "&ServiceId=", sizeof(url) - strlen(url) - 1);
  // strncat(url, status.selectedServiceId.c_str(), sizeof(url) - strlen(url) - 1);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);  // Timeout in ms

  // Prepare JSON body
  char body[150];
  snprintf(body, sizeof(body),
         "{\"device_id\": \"%s\", \"user_id\": %s, \"service_id\": %s}",
         chipIDChar, status.userId, status.selectedServiceId);
  // char body[100];
  // snprintf(body, sizeof(body), "{\"device_id\": \"%s\"}", chipIDChar);

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

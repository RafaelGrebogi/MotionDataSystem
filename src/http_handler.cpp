#include "http_handler.h"
#include "wifi_handler.h"

// WiFiClient client;


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;



//#################################################################
void initiliaseFirebase(){
  config.api_key = FirebaseApiKey;
  config.database_url = FirebaseBaseUrl;

  // /* Sign up */
  // if (Firebase.signUp(&config, &auth, "", "")){
  //   Serial.println("Signed up.");

  // }
  // else{
  //   Serial.printf("%s\n", config.signer.signupError.message.c_str());
  // }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);


}

//#################################################################
void sendDataToFirebase(String jsonData) {
  FirebaseJson json;
  json.setJsonData(jsonData);  // Convert String to FirebaseJson

  if (Firebase.ready()) {
      if (Firebase.RTDB.setJSON(&fbdo, "/ESP32/Data", &json)) {
          Serial.println(" Data sent to Firebase!");
      } else {
          Serial.print(" Firebase Error: ");
          Serial.println(fbdo.errorReason());
      }
  }
}
//#################################################################



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
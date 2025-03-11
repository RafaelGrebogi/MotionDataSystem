#include "http_handler.h"
#include "wifi_handler.h"

// WiFiClient client;

int sendDataToThingSpeak(float value) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(thingSpeakBaseUrl) + "?api_key=" + String(thingSpeakApiKey) + "&field1=" + String(value);
        http.begin(client, url);
        int httpResponseCode = http.GET();

        if (httpResponseCode>0) {
            Serial.print("ThingSpeak Response: ");
            Serial.println(httpResponseCode);
          }
          else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
          }
          String s = http.getString();
          Serial.print(s + "\n");
        
        http.end();
        return httpResponseCode;
    } else {
        Serial.println("WiFi Disconnected");
        return -1;
    }
}
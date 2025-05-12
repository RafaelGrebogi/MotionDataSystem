// #include "wifi_handler.h"

// WiFiClient client;

// void ConnectToWifi(){
//     WiFi.begin(ssid, password);
//     Serial.println("Connecting...");
//     while(WiFi.status() != WL_CONNECTED) {
//       delay(500);
//       Serial.print(".");
//     }
//     Serial.println("");
//     Serial.print("Connected to WiFi network with IP Address: ");
//     Serial.println(WiFi.localIP());

// }



#include "wifi_handler.h"

WiFiClient client;

// Define your Wi-Fi networks (SSID and password pairs)
const char* knownSSIDs[] = {SSID1, SSID2};
const char* knownPasswords[] = {PASSWORD1, PASSWORD2};
const int numNetworks = sizeof(knownSSIDs) / sizeof(knownSSIDs[0]);

// Connect to a known network
bool connect2network(const char* ssid, const char* password) {
    Serial.printf("Attempting to connect to SSID: %s\n", ssid);
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.println("IP address: " + WiFi.localIP().toString());
        return true;
    }

    Serial.println("\nConnection failed.");
    return false;
}

// Search and connect to available networks
void ConnectToWifi() {
    Serial.println("Scanning for available networks...");
    int n = WiFi.scanNetworks();
    Serial.printf("Found %d networks\n", n);

    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        Serial.printf("Found SSID: %s\n", ssid.c_str());

        // Check if this SSID matches any of the known networks
        for (int j = 0; j < numNetworks; j++) {
            if (ssid == knownSSIDs[j]) {
                Serial.printf("Matching SSID found: %s\n", ssid.c_str());
                if (connect2network(knownSSIDs[j], knownPasswords[j])) {
                    return;
                }
            }
        }
    }

    Serial.println("No known networks found. Retrying in 5 seconds...");
    delay(5000);
}

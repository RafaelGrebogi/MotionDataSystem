#include <Arduino.h>
#include <unity.h>
#include "wifi_handler.h"
#include "http_handler.h"

//============================
void setUp(void) {
    // set stuff up here
  }
  
void tearDown(void) {
    // clean stuff up here
  }

//============================
void test_wifi_connection() {
    ConnectToWifi();
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
}


void test_thingspeak_success() {
    ConnectToWifi();
    float testValue = 42.0;
    int responseCode = sendDataToThingSpeak(testValue);  // Use new function name

    TEST_ASSERT_EQUAL_INT(200, responseCode);  // Expect HTTP 200 OK
}

void test_thingspeak_failure() {
    WiFi.disconnect();  // Simulate Wi-Fi failure
    int responseCode = sendDataToThingSpeak(42.0);  

    TEST_ASSERT_EQUAL_INT(-1, responseCode);  // Expect -1 when Wi-Fi is disconnected
}


int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_connection);
    RUN_TEST(test_thingspeak_success);
    RUN_TEST(test_thingspeak_failure);
    return UNITY_END();
  }



//============================
  void setup() {
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);
  
    runUnityTests();
  }
  //============================
  void loop() {}

//===========================================================================

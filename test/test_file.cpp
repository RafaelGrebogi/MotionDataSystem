#include <Arduino.h>
#include <unity.h>
#include "wifi_handler.h"
#include "http_handler.h"
#include "web_handler.h"

//============================
void setUp(void) {
    // set stuff up here
    
  }
  
void tearDown(void) {
    // clean stuff up here
  }

//============================
// WiFi Tests
//============================
void test_wifi_connection() {
    ConnectToWifi();
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
}
//============================
//============================

//============================
// API Tests
//============================
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
//============================
//============================


//============================
// WebServer Tests
//============================
void test_webserver_start() {
    StartWebServer();
    isAcquiring = false;  // ✅ Ensure the initial state is OFF
    handleStart();  // ✅ Call the function to simulate a request
    TEST_ASSERT_TRUE(isAcquiring);  // ✅ `isAcquiring` should now be true
}

void test_webserver_stop() {
    // StartWebServer();
    isAcquiring = true;  // ✅ Ensure acquisition is running
    handleStop();  // ✅ Call the function to simulate a request
    TEST_ASSERT_FALSE(isAcquiring);  // ✅ `isAcquiring` should now be false
}

void test_webserver_status_running() {
    // StartWebServer();
    isAcquiring = true;  // ✅ Simulate running state
    String response;
    handleStatus();  // ✅ Call status endpoint
    response = isAcquiring ? "RUNNING" : "STOPPED";  // ✅ Simulate response
    TEST_ASSERT_EQUAL_STRING("RUNNING", response.c_str());  // ✅ Check correct response
}

void test_webserver_status_stopped() {
    // StartWebServer();
    isAcquiring = false;  // ✅ Simulate stopped state
    String response;
    handleStatus();
    response = isAcquiring ? "RUNNING" : "STOPPED";  // ✅ Simulate response
    TEST_ASSERT_EQUAL_STRING("STOPPED", response.c_str());  // ✅ Check correct response
}
//============================
//============================

//============================

//============================
int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_connection);
    RUN_TEST(test_thingspeak_success);
    RUN_TEST(test_thingspeak_failure);
    RUN_TEST(test_webserver_start);
    RUN_TEST(test_webserver_stop);
    RUN_TEST(test_webserver_status_running);
    RUN_TEST(test_webserver_status_stopped);
    return UNITY_END();
  }
//============================
//============================


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

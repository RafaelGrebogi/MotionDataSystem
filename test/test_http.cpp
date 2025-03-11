#include <Arduino.h>
#include <unity.h>
#include "http_handler.h"
#include "wifi_handler.h"

void test_thingspeak_request() {
    float testValue = 42.0;
    sendDataToThingSpeak(testValue);
    delay(5000);  // ✅ Allow request time to complete
    
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());  // ✅ Wi-Fi should be connected
}

void setup() {
    UNITY_BEGIN();
    // ConnectToWifi();  // ✅ Connect first
    // delay(5000);
    RUN_TEST(test_thingspeak_request);
    UNITY_END();
}

//--------------------
void setUp(void) {  // ✅ Use `setUp()` instead of `setup()`
    ConnectToWifi();
}

void tearDown(void) {}  // ✅ Unity requires this function
//--------------------

void loop() {}  // Required for PlatformIO tests

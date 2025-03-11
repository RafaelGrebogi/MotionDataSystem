#include <Arduino.h>
#include <unity.h>
#include "wifi_handler.h"

void test_wifi_connection() {
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());  // Wi-Fi should be connected
}

void setup() {
    UNITY_BEGIN();
    // ConnectToWifi();  // Connect to Wi-Fi before testing
    // delay(5000);  // Wait for connection
    RUN_TEST(test_wifi_connection);
    UNITY_END();
}


void setUp(void) {  // ✅ Use `setUp()`
    ConnectToWifi();
}

void tearDown(void) {}

void loop() {}  // Required for PlatformIO tests

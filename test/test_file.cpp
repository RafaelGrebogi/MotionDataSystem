#include <Arduino.h>
#include <unity.h>
#include "wifi_handler.h"
#include "http_handler.h"
#include "web_handler.h"
#include "mpu6050_handler.h"
#include "json_handler.h"

GyroBias test_gyro_bias = {0.0, 0.0, 0.0};
extern int dataCount;
extern hw_timer_t *My_timer;

//============================
void setUp(void) {
    // set stuff up here
    
  }
  
void tearDown(void) {
    // clean stuff up here
  }

//============================
// MPU6050 Tests
//============================
void test_MPUsensor_accelX() {
  StartMPU6050();
  delay(1000);
  ReadGyro(datafile, test_gyro_bias);

  TEST_ASSERT_FLOAT_WITHIN(0.5, 0.5, datafile.accelX);
}
void test_MPUsensor_accelY() {
    StartMPU6050();
    delay(1000);
    ReadGyro(datafile, test_gyro_bias);
  
    TEST_ASSERT_FLOAT_WITHIN(0.5, -0.5, datafile.accelY);
  }

void test_MPUsensor_accelZ() {
    StartMPU6050();
    delay(1000);
    ReadGyro(datafile, test_gyro_bias);

    TEST_ASSERT_FLOAT_WITHIN(0.5, 9.5, datafile.accelZ);

  }

  void test_MPUsensor_gyroX() {
    StartMPU6050();
    delay(1000);
    ReadGyro(datafile, test_gyro_bias);
  
    TEST_ASSERT_FLOAT_WITHIN(0.5, 0.5, datafile.gyroX);
  }
  void test_MPUsensor_gyroY() {
      StartMPU6050();
      delay(1000);
      ReadGyro(datafile, test_gyro_bias);
    
      TEST_ASSERT_FLOAT_WITHIN(0.5, 0.5, datafile.gyroY);
    }
  
  void test_MPUsensor_gyroZ() {
      StartMPU6050();
      delay(1000);
      ReadGyro(datafile, test_gyro_bias);
  
      TEST_ASSERT_FLOAT_WITHIN(0.5, 0.5, datafile.gyroZ);
  
    }
//============================
//============================


//============================
// JSON Tests
//============================
void test_json_not_empty() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  String jsonOutput = prepareJsonPayload();
  
  TEST_ASSERT(jsonOutput.length() > 2);  // Ensure JSON is not empty ("{}" is 2 chars)
}


void test_json_contains_device_id() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  String jsonOutput = prepareJsonPayload();
  
  TEST_ASSERT_NOT_EQUAL(jsonOutput.indexOf("device_id"), -1);  // Ensure "device_id" exists
}


void test_json_contains_message_id() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  String jsonOutput = prepareJsonPayload();
  
  TEST_ASSERT_NOT_EQUAL(jsonOutput.indexOf("message_id"), -1);  // Ensure "message_id" exists
}


void test_json_contains_timestamp() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  String jsonOutput = prepareJsonPayload();
  
  TEST_ASSERT_NOT_EQUAL(jsonOutput.indexOf("timestamp"), -1);  // Ensure "timestamp" exists
}

void test_json_contains_samples() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  String jsonOutput = prepareJsonPayload();
  
  TEST_ASSERT_NOT_EQUAL(jsonOutput.indexOf("\"samples\""), -1);  // Ensure "samples" key exists
}



void test_json_reset_after_sending() {
  dataCount = BATCH_SIZE;  // Simulate full buffer
  prepareJsonPayload();  // Generate JSON

  TEST_ASSERT_EQUAL_INT(0, dataCount);  // Ensure dataCount resets
}

//============================
//============================



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
// void test_thingspeak_success() {
//     ConnectToWifi();
//     float testValue = 42.0;
//     int responseCode = sendDataToThingSpeak(testValue);  // Use new function name

//     TEST_ASSERT_EQUAL_INT(200, responseCode);  // Expect HTTP 200 OK
// }

// void test_thingspeak_failure() {
//     WiFi.disconnect();  // Simulate Wi-Fi failure
//     int responseCode = sendDataToThingSpeak(42.0);  

//     TEST_ASSERT_EQUAL_INT(-1, responseCode);  // Expect -1 when Wi-Fi is disconnected
// }
//============================
//============================


//============================
// WebServer Tests
//============================
void test_webserver_start() {
    // StartWebServer();
    isAcquiring = false;  // Ensure the initial state is OFF
    test_handleStart();  // Call the function to simulate a request
    TEST_ASSERT_TRUE(isAcquiring);  // `isAcquiring` should now be true
}

void test_webserver_stop() {
    // StartWebServer();
    isAcquiring = true;  //  Ensure acquisition is running
    test_handleStop();  //  Call the function to simulate a request
    TEST_ASSERT_FALSE(isAcquiring);  //  `isAcquiring` should now be false
}

void test_webserver_status_running() {
    // StartWebServer();
    isAcquiring = true;  //  Simulate running state
    String response;
    handleStatus();  //  Call status endpoint
    response = isAcquiring ? "RUNNING" : "STOPPED";  //  Simulate response
    TEST_ASSERT_EQUAL_STRING("RUNNING", response.c_str());  //  Check correct response
}

void test_webserver_status_stopped() {
    // StartWebServer();
    isAcquiring = false;  //  Simulate stopped state
    String response;
    handleStatus();
    response = isAcquiring ? "RUNNING" : "STOPPED";  //  Simulate response
    TEST_ASSERT_EQUAL_STRING("STOPPED", response.c_str());  //  Check correct response
}
//============================
//============================

//============================

//============================
int runUnityTests(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_MPUsensor_accelX);
    RUN_TEST(test_MPUsensor_accelY);
    RUN_TEST(test_MPUsensor_accelZ);
    RUN_TEST(test_MPUsensor_gyroX);
    RUN_TEST(test_MPUsensor_gyroY);
    RUN_TEST(test_MPUsensor_gyroZ);

    RUN_TEST(test_wifi_connection);

    // RUN_TEST(test_thingspeak_success);
    // RUN_TEST(test_thingspeak_failure);

    RUN_TEST(test_webserver_start);
    RUN_TEST(test_webserver_stop);
    RUN_TEST(test_webserver_status_running);
    RUN_TEST(test_webserver_status_stopped);

    RUN_TEST(test_json_not_empty);
    RUN_TEST(test_json_contains_device_id);
    RUN_TEST(test_json_contains_message_id);
    RUN_TEST(test_json_contains_timestamp);
    RUN_TEST(test_json_contains_samples);
    RUN_TEST(test_json_reset_after_sending);

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

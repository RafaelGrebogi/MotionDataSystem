
#ifndef PIO_UNIT_TESTING

//-------------------------------------
// LIBRARIES
//-------------------------------------
#include <Arduino.h>
#include <unity.h>
#include <stdio.h>
#include <stdint.h> // To handle string conversion

#include "config.h"  // Include global configuration

bool TRAINING_MODE = false;  // Default mode | This value is read from an input pin (GPIO)

bool TESTING_MODE = false; // Default mode | This value is read from an input pin (GPIO)

volatile bool modeChanged = false;
unsigned long lastModeChangeTime = 0;
const unsigned long debounceInterval = 3000;  // 3 seconds
volatile bool serverNeedsUpdate = true;        // Force first setup

OperationMode lastMode = TRAINING;

UserStatus status;



#include "time.h"

// Hardware Abstract Layer library
#include "esp32-hal-cpu.h"

// JSON document library
#include "ArduinoJson.h"

#include <Firebase_ESP_Client.h>
// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"



#include "mpu6050_handler.h"
#include "wifi_handler.h"
#include "http_handler.h"
#include "web_handler.h"
#include "json_handler.h"
#include "calibration_handler.h"

// WiFi library
#include <WiFi.h>


#include <HTTPClient.h>

// Web Server Library
#include <WebServer.h>

// MPU 6050 libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <FS.h>

// Gyroscope bias variable
// GyroBias gyroBias;

// Chip ID number
extern char chipIDChar[16];

// Current time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 10*3600;  // Adjust for your timezone
const int daylightOffset_sec = 3600;



//-------------------------------------
// Functions 
//-------------------------------------
// Bridging function
UserStatus safeFetchUserId(String username) {

  UserStatus status = fetchUserStatusFromAPI(username,chipIDChar);

  return status;
}




// Bridging function
void safeTriggerFastAPI() {
  // Send 'complete' flag to Firebase to inform FastAPI of data available
  sendCompleteFlag();

  // Trigger FastAPI to read data from Firebase
  triggerFastAPI();
}

//---------------------

// Mode selection function
OperationMode readModePin(){
  int trainingState = digitalRead(TRAINING_MODE_PIN);
  int testingState = digitalRead(TESTING_MODE_PIN);

  // Check pin states (active LOW due to INPUT_PULLUP)
  if (trainingState == HIGH && testingState == LOW) {
      currentMode = TRAINING;
      TESTING_MODE = false;
      TRAINING_MODE = true;
      Serial.println("Mode: Training");

  } else if (trainingState == HIGH && testingState == HIGH) {
      currentMode = TESTING;
      TESTING_MODE = true;
      TRAINING_MODE = false;
      Serial.println("Mode: Testing");
  } else {
      currentMode = PRODUCTION;
      TRAINING_MODE = false;
      TESTING_MODE = false;
      Serial.println("Mode: Production");
  }
  return currentMode;
} 

//---------------------

// Check if the elapsed time is greater than or equal to the interval
bool isElapsed(unsigned long *lastTime, unsigned long interval) {
    unsigned long currentTime = millis();
    
    if (currentTime - *lastTime >= interval) {
        *lastTime = currentTime;  // Update the last time
        return true;              // Time has elapsed
    }
    return false;                 // Time has not elapsed
}


//-------------------------------------
// INTERRUPTION SERVICE ROUTINE VARIABLES 
//-------------------------------------
// Timer_0 for ISR 
hw_timer_t *My_timer = NULL;
bool ISRTimer0 = false;
uint8_t counter100 = 0;



//----------------------------------------------------------------- 
//----------------- Interruption Service Routine ------------------
//----------------------------------------------------------------- 
void IRAM_ATTR onTimer(){

  ISRTimer0 = true;   // can change to bitwise operator (~) in the future
}


void IRAM_ATTR handleModeChange() {
  lastModeChangeTime = millis();  // record the moment of change
  modeChanged = true;  // Indicate that the mode has changed

}

//-------------------------------------
void setup() {
  // put your setup code here, to run once:
    // Initialise Serial0 to show in Terminal
  Serial.begin(115200);
  Serial.println("Initialising...");


  pinMode(TRAINING_MODE_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(TESTING_MODE_PIN, INPUT_PULLUP);  // Use internal pull-up resistor

  // Attach interrupts to both pins (CHANGE mode to detect both rising and falling edges)
  attachInterrupt(digitalPinToInterrupt(TRAINING_MODE_PIN), handleModeChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TESTING_MODE_PIN), handleModeChange, CHANGE);
  Serial.println("Mode Selection Initialised");
  currentMode = readModePin();
  lastMode = currentMode;




  // Set CPU frequency
  setCpuFrequencyMhz(80);
  Serial.print("CPU Frequency (MHz): ");

  // Get CPU frequency
  Serial.println(getCpuFrequencyMhz());
  

  
  #ifdef FAKE_ID
  // Simulate a different device for testing
    snprintf(chipIDChar, sizeof(chipIDChar), "TEST_DEVICE_1");
    Serial.printf("ESP32 ID = %s\n", chipIDChar);
  #else
    // Get Chip ID
    strcpy(chipIDChar, ESP32_ID_Extraction());
  #endif
  
  // Initialise Wi-Fi
  ConnectToWifi();

  // Config device to get current time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // Get time from NTP
 

  // Initialise Firebase
  initiliaseFirebase();

  // Fetch FastAPI IP address
  if (!fetchFastApiIP()) {
    char message[100];
    snprintf(message, sizeof(message), "⚠️ Using default FastAPI IP:  %s", fastapi_ip);
    Serial.println(message);
  }

  // Initialise WebServer
  InitialWebServerSetup();
  // if(currentMode == TRAINING || currentMode == TESTING){
  //   StartWebServerTRAIN(); // Training path
  // }else{
  //   Serial.println("Production mode");
  //   StartWebServer(); // Production path
  // }

  // Initialise MPU6050
  StartMPU6050();

  Serial.println("Calibrating Gyroscope...");
  gyroBias = CalibrateGyro();

  Serial.println("Calibration complete.");
  Serial.print("Bias X: "); Serial.println(gyroBias.x);
  Serial.print("Bias Y: "); Serial.println(gyroBias.y);
  Serial.print("Bias Z: "); Serial.println(gyroBias.z);


  // Initiliase Timer_0 for ISR
  My_timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 (1µs resolution)
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 10000, true);  //  100Hz interrupt (10ms interval)
  
  Serial.println("System Initialised!");

  // MPU6050 DEBUG FUNCTION
  // while(1){
  //   ReadAndPrintData();
  // }



}

void loop() {

  server.handleClient();
  
  // Verify if aonther mode was selected
  if (modeChanged && isElapsed(&lastModeChangeTime, debounceInterval)) {
    // Check if the debounce interval has passed
    
    // Reset the flag after processing
    modeChanged = false;

    // Perform mode change action
    currentMode = readModePin();

    // Only reconfigure endpoints if mode changed
    if (currentMode != lastMode) {
      lastMode = currentMode;
      serverNeedsUpdate = true;
      }
    }

    if (serverNeedsUpdate) {
      serverNeedsUpdate = false;

      // Clear old routes (optional but good practice)
      server.close();

      // Register mode-specific endpoints (except "/")
      if (currentMode == TRAINING || currentMode == TESTING) {
        StartWebServerTRAIN();  // Registers /train_* etc.
      } else {
        StartWebServer();       // Registers /start, /stop, etc.
      }

      // Serial.println("Server endpoints updated for current mode.");
    }

  


  // 100Hz
  if(ISRTimer0){                // Boolean var toggled in Timer0 interruption

    ReadGyro(datafile,gyroBias);       // Read data from gyroscope / acceloremeter (100Hz)

    // store data in json
    collectData(targetLabel ,datafile.accelX,datafile.accelY, datafile.accelZ, datafile.gyroX, datafile.gyroY, datafile.gyroZ, getCurrentTimestamp());

    counter100++;


    // // 10Hz
    // if(counter100%10 == 0){
    //   //Serial.println("Hey10");

    // }
    if(counter100 == 100){
      counter100 = 0;
      Serial.println("🔴 Data Acquisition Running...");;
    }

    ISRTimer0 = false;  // can change to bitwise operator (~) in the future

  }


  // float sensorValue = random(10, 100);  // Replace with real sensor data
  // // sendDataToThingSpeak(sensorValue);
  
  // Serial.print("Sent value: ");
  // Serial.println(sensorValue);

  // // delay(17000);  // Wait 17 seconds before next update


}


#endif 


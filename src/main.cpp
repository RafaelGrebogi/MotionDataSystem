
#ifndef PIO_UNIT_TESTING

//-------------------------------------
// LIBRARIES
//-------------------------------------
#include <stdio.h>
#include <stdint.h> // To handle string conversion

#include "time.h"

// Hardware Abstract Layer library
#include "esp32-hal-cpu.h"

// JSON document library
#include "ArduinoJson.h"

#include <Arduino.h>
#include <unity.h>

#include "mpu6050_handler.h"
#include "wifi_handler.h"
#include "http_handler.h"
#include "web_handler.h"
#include "json_handler.h"

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
GyroBias gyro_bias;

// Chip ID number
extern char chipIDChar[16];


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 10*3600;  // Adjust for your timezone
const int daylightOffset_sec = 3600;


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




//-------------------------------------
void setup() {
  // put your setup code here, to run once:
    // Initialise Serial0 to show in Terminal
  Serial.begin(115200);
  Serial.println("Initialising...");

  // Set CPU frequency
  setCpuFrequencyMhz(80);
  Serial.print("CPU Frequency (MHz): ");

  // Get Chip ID
  Serial.println(getCpuFrequencyMhz());
  strcpy(chipIDChar, ESP32_ID_Extraction());
  Serial.println(chipIDChar);
  // Initialise Wi-Fi
  ConnectToWifi();

  // Config device to get current time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // Get time from NTP

  // Initialise WebServer
  StartWebServer();

  // Initialise MPU6050
  StartMPU6050();

  Serial.println("Calibrating Gyroscope...");
  gyro_bias = CalibrateGyro();

  Serial.println("Calibration complete.");
  Serial.print("Bias X: "); Serial.println(gyro_bias.x);
  Serial.print("Bias Y: "); Serial.println(gyro_bias.y);
  Serial.print("Bias Z: "); Serial.println(gyro_bias.z);


  // Initiliase Timer_0 for ISR
  My_timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 (1µs resolution)
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 10000, true);  //  100Hz interrupt (10ms interval)
  
  Serial.println("System Initialised!");
}

void loop() {

server.handleClient();

  // 100Hz
  if(ISRTimer0){                // Boolean var toggled in Timer0 interruption

    ReadGyro(datafile,gyro_bias);       // Read data from gyroscope / acceloremeter (100Hz)

    // store data in json
    collectData(datafile.accelX,datafile.accelY, datafile.accelZ, datafile.gyroX, datafile.gyroY, datafile.gyroZ, getCurrentTimestamp());

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


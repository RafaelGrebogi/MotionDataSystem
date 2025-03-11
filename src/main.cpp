
#ifndef PIO_UNIT_TESTING



#include <Arduino.h>
#include <unity.h>

#include "mpu6050_handler.h"
#include "wifi_handler.h"
#include "http_handler.h"
#include "web_handler.h"

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

//-------------------------------------
// GYROSCOPE / ACCELOREMETER (MPU6050)
//-------------------------------------
// Adafruit_MPU6050 mpu;
// sensors_event_t a, g, temp;




//-------------------------------------
// Variables
//-------------------------------------
// struct DataFile {
//   char accelX[16];
//   char accelY[16];
//   char accelZ[16];
//   char gyroX[16];
//   char gyroY[16];
//   char gyroZ[16];
// };
// DataFile datafile;       // <- global datafile object

// extern DataFile datafile;




//-------------------------------------
void setup() {
  // put your setup code here, to run once:
    // Initialise Serial0 to show in Terminal
  Serial.begin(115200);
  Serial.println("Initialising...");

  // Set CPU frequency
  setCpuFrequencyMhz(80);
  Serial.print("CPU Frequency (MHz): ");
  Serial.println(getCpuFrequencyMhz());

  // Initialise Wi-Fi
  ConnectToWifi();

  // Initialise WebServer
  StartWebServer();

  // Initialise MPU6050
  StartMPU6050();
  
  Serial.println("Initialised!");
}

void loop() {
 
  // Serial.println("Sampling...");
  // ReadGyro(datafile);       // Read data from gyroscope / acceloremeter (100Hz)
  // delay(2000);

  // float sensorValue = random(10, 100);  // Replace with real sensor data
  // // sendDataToThingSpeak(sensorValue);
  
  // Serial.print("Sent value: ");
  // Serial.println(sensorValue);

  // // delay(17000);  // Wait 17 seconds before next update

  server.handleClient();
  
  

  if (isAcquiring) {  // ✅ Start acquisition when START is clicked
    Serial.println("🔴 Data Acquisition Running...");
    delay(1000);  // ✅ Simulate data acquisition (replace with real logic)
}


}



#endif 


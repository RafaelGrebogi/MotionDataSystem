#ifndef MPU6050_HANDLER_H
#define MPU6050_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <FS.h>
#include "calibration_handler.h"

extern Adafruit_MPU6050 mpu;  // MPU6050 object
extern sensors_event_t a, g, temp;

struct GyroBias {
  float x;
  float y;
  float z;
};



struct DataFile {
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  };
// DataFile datafile;       // <- global datafile object

extern DataFile datafile;


void StartMPU6050();
void ReadGyro(DataFile &datafile, GyroBias &gyro_bias);
GyroBias CalibrateGyro();

#endif  // MPU6050_HANDLER_H
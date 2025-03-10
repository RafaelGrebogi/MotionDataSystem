#ifndef MPU6050_HANDLER_H
#define MPU6050_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <FS.h>

extern Adafruit_MPU6050 mpu;  // MPU6050 object
extern sensors_event_t a, g, temp;

struct DataFile {
    char accelX[16];
    char accelY[16];
    char accelZ[16];
    char gyroX[16];
    char gyroY[16];
    char gyroZ[16];
  };
// DataFile datafile;       // <- global datafile object

extern DataFile datafile;


void StartMPU6050();
void ReadGyro(DataFile &datafile);

#endif  // MPU6050_HANDLER_H
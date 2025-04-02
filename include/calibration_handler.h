#ifndef CALIBRATION_HANDLER_H
#define CALIBRATION_HANDLER_H

// MPU 6050 libraries
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <FS.h>
#include <math.h>

// Tilt angles (in radians) and calibration flag
extern float tiltX;
extern float tiltY;
extern bool isCalibrated;

#define ACC_SCALE_FACTOR 4096.0

extern Adafruit_MPU6050 mpu;
extern sensors_event_t a, g, temp;

// Function to calibrate the accelerometer at rest
void CalibrateAccelerometer(int samples = 100);

// Function to return corrected acceleration values
void GetCorrectedAcceleration(float x, float y, float z, float &xCorr, float &yCorr, float &zCorr);

#endif
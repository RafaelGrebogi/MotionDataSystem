#include "calibration_handler.h"



// Adafruit_MPU6050 mpu;  // Define MPU6050 object
// sensors_event_t a, g, temp;


// Calibration parameters
float tiltX = 0, tiltY = 0;        // radians
float sinTiltX = 0, cosTiltX = 1;
float sinTiltY = 0, cosTiltY = 1;
bool isCalibrated = false;



void CalibrateAccelerometer(int samples) {
    float sumX = 0, sumY = 0, sumZ = 0;

    Serial.println(" Starting accelerometer calibration... Stand still.");
    

    for (int i = 0; i < samples; i++) {
        mpu.getEvent(&a, &g, &temp);
        sumX += a.acceleration.x;
        sumY += a.acceleration.y;
        sumZ += a.acceleration.z;
        delay(10);
    }

    float biasX = sumX / samples;
    float biasY = sumY / samples;

    // Convert to tilt angles
    tiltX = asin(biasX / 16384.0);  // sagittal tilt
    tiltY = asin(biasY / 16384.0);  // frontal tilt

    sinTiltX = sin(tiltX);
    cosTiltX = cos(tiltX);
    sinTiltY = sin(tiltY);
    cosTiltY = cos(tiltY);

    isCalibrated = true;

    Serial.println("✅ Calibration complete.");
    Serial.print("TiltX (deg): "); Serial.println(tiltX * 180.0 / PI);
    Serial.print("TiltY (deg): "); Serial.println(tiltY * 180.0 / PI);
}



void GetCorrectedAcceleration(float x, float y, float z, float &xCorr, float &yCorr, float &zCorr) {
    if (!isCalibrated) {
        xCorr = x;
        yCorr = y;
        zCorr = z;
        return;
    }

    // Apply precomputed rotation correction
    float z1 = z * cosTiltX - y * sinTiltX;
    float y1 = z * sinTiltX + y * cosTiltX;

    float z2 = z1 * cosTiltY + x * sinTiltY;
    float x1 = z1 * sinTiltY + x * cosTiltY;

    xCorr = x1;
    yCorr = y1;
    zCorr = z2;
}


#include "calibration_handler.h"



// Adafruit_MPU6050 mpu;  // Define MPU6050 object
// sensors_event_t a, g, temp;


// Calibration parameters
float tiltX = 0, tiltY = 0;        // radians
float sinTiltX = 0, cosTiltX = 1;
float sinTiltY = 0, cosTiltY = 1;
bool isCalibrated = false;


void ReadAndPrintData(){

    // Serial.println(" Tilt board. Start in 5s.");
    // delay(5000);

    mpu.getEvent(&a, &g, &temp);
    /* Print out the values */
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

    Serial.println("");

    CalibrateAccelerometer(100);

    Serial.println("DONE!");

    delay(1000);

}





void CalibrateAccelerometer(int samples) {
    float sumX = 0, sumY = 0, sumZ = 0;

    Serial.println(" Starting accelerometer calibration... Hold still.");

    for (int i = 0; i < samples; i++) {
        mpu.getEvent(&a, &g, &temp);
        sumX += a.acceleration.x;
        sumY += a.acceleration.y;
        sumZ += a.acceleration.z;
        delay(10);
    }

    float biasX = sumX / samples;
    float biasY = sumY / samples;
    float biasZ = sumZ / samples;

    // Compute tilt angles using atan2
    // Sagittal (forward-back) tilt: rotation around Y (tiltX)
    tiltX = atan2(biasX, sqrt(biasY * biasY + biasZ * biasZ));

    // Frontal (side-to-side) tilt: rotation around X (tiltY)
    tiltY = atan2(biasY, sqrt(biasX * biasX + biasZ * biasZ));

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

    // First rotate around X-axis (tiltX - pitch correction)
    float y1 = y * cosTiltX - z * sinTiltX;
    float z1 = y * sinTiltX + z * cosTiltX;

    // Then rotate around Y-axis (tiltY - roll correction)
    float x1 = x * cosTiltY + z1 * sinTiltY;
    float z2 = -x * sinTiltY + z1 * cosTiltY;

    xCorr = x1;
    yCorr = y1;
    zCorr = z2;
}



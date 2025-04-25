#include "calibration_handler.h"



// Adafruit_MPU6050 mpu;  // Define MPU6050 object
// sensors_event_t a, g, temp;


// Calibration parameters
float tiltX = 0, tiltY = 0;        // radians
// float sinTiltX = 0, cosTiltX = 1;
// float sinTiltY = 0, cosTiltY = 1;
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

    accelCorr.sinTiltX = sin(tiltX);
    accelCorr.cosTiltX = cos(tiltX);
    accelCorr.sinTiltY = sin(tiltY);
    accelCorr.cosTiltY = cos(tiltY);

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

    float sx = accelCorr.sinTiltX;
    float cx = accelCorr.cosTiltX;
    float sy = accelCorr.sinTiltY;
    float cy = accelCorr.cosTiltY;


    // Full rotation matrix: R = Ry(tiltX) * Rx(tiltY)
    float r11 =  cx;
    float r12 =  sx * sy;
    float r13 =  sx * cy;

    float r21 =  0;
    float r22 =      cy;
    float r23 =     -sy;

    float r31 = -sx;
    float r32 =  cx * sy;
    float r33 =  cx * cy;

    // Apply the rotation matrix
    xCorr = r11 * x + r12 * y + r13 * z;
    yCorr = r21 * x + r22 * y + r23 * z;
    zCorr = r31 * x + r32 * y + r33 * z;
}


//##############################################################
GyroBias CalibrateGyro(){

    int counter = 0, num_samples = 1000; // Number of samples for calibration
    float sum_x = 0, sum_y = 0, sum_z = 0;

    while(counter < num_samples){
        mpu.getEvent(&a, &g, &temp);
        counter++;
 
        sum_x += g.gyro.x;
        sum_y += g.gyro.y;
        sum_z += g.gyro.z;

        delay(10);
    }
   
  // Return bias as a struct
  return {sum_x / num_samples, sum_y / num_samples, sum_z / num_samples};
}
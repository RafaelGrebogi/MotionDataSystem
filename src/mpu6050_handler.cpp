#include "mpu6050_handler.h"

Adafruit_MPU6050 mpu;  // Define MPU6050 object
sensors_event_t a, g, temp;

extern hw_timer_t *My_timer;
extern bool ISRTimer0;

DataFile datafile;





//##############################################################
// Initialise MPU6050
void StartMPU6050() {

    // Try to initialize!
    if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("MPU6050 Found!");
  
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
      case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
      case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
      case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
      case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
      case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
      case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
      case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
      case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }
  
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
      case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
      case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
      case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
      case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
      case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
      case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
      case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }
  
    Serial.println("");
    delay(100);
}
  
  
  //##############################################################
  // Read MPU6050
  void ReadGyro(DataFile &datafile, GyroBias &gyro_bias) {
    mpu.getEvent(&a, &g, &temp);

    // Apply accelerometer tilt correction
    float correctedX, correctedY, correctedZ;
    GetCorrectedAcceleration(
        a.acceleration.x,
        a.acceleration.y,
        a.acceleration.z,
        correctedX,
        correctedY,
        correctedZ
    );

    // Store corrected accelerometer data
    datafile.accelX = correctedX;
    datafile.accelY = correctedY;
    datafile.accelZ = correctedZ;

    // Apply gyroscope bias correction
    datafile.gyroX = g.gyro.x - gyro_bias.x;
    datafile.gyroY = g.gyro.y - gyro_bias.y;
    datafile.gyroZ = g.gyro.z - gyro_bias.z;
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
  
  
#include "mpu6050_handler.h"

Adafruit_MPU6050 mpu;  // Define MPU6050 object
sensors_event_t a, g, temp;

extern hw_timer_t *My_timer;

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
  void ReadGyro(DataFile &datafile){
  
    
    mpu.getEvent(&a, &g, &temp);
  
    sprintf(datafile.accelX,"%06f",a.acceleration.x); 
    sprintf(datafile.accelY,"%06f",a.acceleration.y); 
    sprintf(datafile.accelZ,"%06f",a.acceleration.z); 
    sprintf(datafile.gyroX,"%06f",g.gyro.x); 
    sprintf(datafile.gyroY,"%06f",g.gyro.y); 
    sprintf(datafile.gyroZ,"%06f",g.gyro.z); 
  
    //sprintf(datafile.accelX,"%02f",a.acceleration.x); 
    // Serial.println(datafile.accelX);
    // Serial.println(datafile.accelY);
    // Serial.println(datafile.accelZ);
    // Serial.println(datafile.gyroX);
    // Serial.println(datafile.gyroY);
    // Serial.println(datafile.gyroZ);
  
  
  }


  void CalibrateGyro(){
    
    timerAlarmEnable(My_timer);

  }
  
  
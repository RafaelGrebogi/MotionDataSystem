# Motion Data System

Embedded system developed for human motion data acquisition and analysis using an **ESP32** and **MPU6050 IMU**.

The device collects accelerometer and gyroscope measurements at **100 Hz** and was developed as part of an end-to-end system for motion analysis and machine learning.

## Embedded System

The firmware is written in **C++ using PlatformIO** and includes:

* MPU6050 accelerometer and gyroscope acquisition
* Sensor calibration and gyroscope bias correction
* Device orientation correction
* Timer-based 100 Hz sampling
* Interrupt handling
* Different operating modes for data collection and testing
* Wi-Fi and HTTP communication
* Firebase communication for device control and data exchange

The firmware is organised into separate modules for sensor acquisition, calibration, communication and device control.

## System Structure

The ESP32 acquires and processes the IMU measurements before sending the data to the software backend over Wi-Fi.

The backend is implemented in Python using **FastAPI** and handles data processing, feature extraction, database communication and the machine-learning components of the system. Firebase and Supabase are used for communication and data storage.

The backend is available in a separate repository:

[fastapi-ml-api](https://github.com/RafaelGrebogi/fastapi-ml-api)

## Hardware and Software

**Hardware:** ESP32, MPU6050 IMU
**Embedded:** C++, PlatformIO
**Backend:** Python, FastAPI
**Data:** Firebase, Supabase/PostgreSQL
**Machine Learning:** Python, scikit-learn

## Repository

The main firmware is located in `src/`, with header files in `include/`. The project uses PlatformIO for building, uploading and testing the ESP32 firmware.

This project is under ongoing development.

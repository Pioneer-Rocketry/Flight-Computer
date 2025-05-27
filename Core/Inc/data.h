
#ifndef DATA_H
#define DATA_H

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

class Data {
public:
    Data();

    void save();
    void log();

    // Raw sensor readings
    Vector LSM6DSV320_LowG_Accel;  // Low G  Accelerometer measurements in m/s^2
    Vector LSM6DSV320_HighG_Accel; // High G Accelerometer measurements in m/s^2

    Vector LSM6DSV320_Gyro; // Gyroscope measurements in degrees/s

    Vector MMC5603_Mag; // MMC5603 Magnetometer measurements in milliGauss

    float MS560702BA03_Pressure; // MS560702BA03 Pressure measurements in hPa
    float MS560702BA03_Altitude; // MS560702BA03 altitude in m using https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf to convert

    // State Estimation
    Quaternion orientation; // Quaternion representing of orientation
};

#endif

#ifndef DATA_H
#define DATA_H

#include <stdio.h> // for snprintf

#include "stm32f4xx_hal.h"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

#define GRAVITY     9.80665f
#define PI          3.141592653589793
#define RAD_TO_DEG  180/PI
#define DEG_TO_RAD  PI/180
#define FT_TO_M     0.3048

class Data {
private:
    char buffer[512];

public:
    Data(UART_HandleTypeDef* uart);

    void save();
    void log();

    // Raw sensor readings
    Vector LSM6DSV320_LowG_Accel;  // Low G  Accelerometer measurements in m/s^2
    Vector LSM6DSV320_HighG_Accel; // High G Accelerometer measurements in m/s^2

    Vector LSM6DSV320_Gyro; // Gyroscope measurements in degrees/s

    Vector MMC5603_Mag; // MMC5603 Magnetometer measurements in milliGauss

    float MS5607_Pressure;    // MS560702BA03 Pressure measurements in hPa
    float MS5607_Temperature; // MS560702BA03 Temperature measurements in hPa
    float MS5607_Altitude;    // MS560702BA03 altitude in m using https://www.weather.gov/media/epz/wxcalc/pressureAltitude.pdf to convert

    float startingAltitude;

    // State Estimation
    Quaternion orientation; // Quaternion representing of orientation
    Vector eular;

    Vector position; // Vector representing of position
    Vector velocity; // Vector representing of velocity

    uint32_t timestamp;
    float dt;
    float freq;
    // int timestamp;

    UART_HandleTypeDef* uart;
};

#endif
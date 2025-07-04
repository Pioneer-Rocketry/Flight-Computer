
#ifndef DATA_H
#define DATA_H

#include <stdio.h> // for snprintf

#include "stm32f4xx_hal.h"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

#include "definitions.h"


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

    Vector rotated_LowG_Accel;
    Vector rotated_HighG_Accel;

    Vector position; // Vector representing of position
    Vector velocity; // Vector representing of velocity

    // Time
    uint32_t timestamp;
    float dt;
    float freq;
    // int timestamp;

    uint8_t state;

    // UART
    UART_HandleTypeDef* uart;
};

#endif
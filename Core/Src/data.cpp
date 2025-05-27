
#include "data.h"

Data::Data() {
    // Zero all data
    this->LSM6DSV320_LowG_Accel.zero();
    this->LSM6DSV320_HighG_Accel.zero();
    this->LSM6DSV320_Gyro.zero();
    this->MMC5603_Mag.zero();

    this->MS560702BA03_Pressure = 0;
    this->MS560702BA03_Altitude = 0;
}

void Data::save() {
}

void Data::log() {
}
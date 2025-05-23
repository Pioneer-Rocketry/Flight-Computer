
#include "data.h"

Data::Data() {
    this->LSM6DSV320_LowG_Accel.zero();
    this->LSM6DSV320_HighG_Accel.zero();
    this->LSM6DSV320_Gyro.zero();
    this->MMC5603_Mag.zero();
}

void Data::save() {
}

void Data::log() {
}
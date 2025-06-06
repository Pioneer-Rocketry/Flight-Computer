#include "filters/orientation_filter.h"

Orientation_Filter::Orientation_Filter(Data *data)
    : Filter(data) {
}

void Orientation_Filter::init() {
}

void Orientation_Filter::compute() {
    dt = (micros() - last_update) / 1000000.0f;
    last_update = micros();

    deltaQuaternion = Quaternion(
        data->LSM6DSV320_Gyro.getX() * DEG_TO_RAD,
        data->LSM6DSV320_Gyro.getY() * DEG_TO_RAD,
        data->LSM6DSV320_Gyro.getZ() * DEG_TO_RAD,
        0
    );
    deltaQuaternion = (data->orientation * deltaQuaternion) * 0.5f;

    data->orientation = data->orientation + deltaQuaternion * dt;
    data->orientation.normalize();
}
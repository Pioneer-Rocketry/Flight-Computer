#include "filters/orientation_filter.h"

Orientation_Filter::Orientation_Filter(Data *data)
    : Filter(data) {
}

void Orientation_Filter::init() {
}

void Orientation_Filter::compute() {
    dt = (micros() - last_update) / 1000000.0f;
    last_update = micros();

    if (dt > 0.1f) return;

    deltaQuaternion = Quaternion(
        0,
        data->LSM6DSV320_Gyro.x * DEG_TO_RAD,
        data->LSM6DSV320_Gyro.y * DEG_TO_RAD,
        data->LSM6DSV320_Gyro.z * DEG_TO_RAD
    );
    deltaQuaternion = (data->orientation * deltaQuaternion) * 0.5f;

    data->orientation = data->orientation + deltaQuaternion * dt;
    data->orientation.normalize();
}
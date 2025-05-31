#include "filters/orientation_filter.h"

Orientation_Filter::Orientation_Filter(Data *data)
    : Filter(data) {
}

void Orientation_Filter::init() {
}

void Orientation_Filter::compute() {
    dt = micros() - last_update;

    norm = this->data->LSM6DSV320_Gyro.magnitude();
    norm = copysignf(fmax(abs(norm), 1e-9), norm); // Avoid division by zero

    deltaQuaternion.fromAxisAngle((this->data->LSM6DSV320_Gyro * DEG_TO_RAD) / norm, dt / norm);
    this->data->orientation *= deltaQuaternion;

    last_update = micros();
}
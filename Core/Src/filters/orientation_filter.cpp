#include "filters/orientation_filter.h"

Orientation_Filter::Orientation_Filter(Data *data)
    : Filter(data) {

    gyroMagnitude = 0.0f;
    theta = 0.0f;

    angularVelocity.zero();

    oldGyroTime = 0.0f;
    newGyroTime = 0.0f;
}

/**
 * Compute the orientation as a quaternion based on angulare velocities measured from the gyroscope
 *
 * Based on @kev1n8088's code in the BPS.Space Patreon Discord Server
 * https://discord.com/channels/355526822760415242/355570980053319692/1376708812701958295
 */
void Orientation_Filter::compute() {
    newGyroTime = micros();
    this->dt = (newGyroTime - oldGyroTime) / 1000000.0; //convert to seconds
    oldGyroTime = newGyroTime;

    this->gyroMagnitude = this->data->LSM6DSV320_Gyro.magnitude();

    if (this->gyroMagnitude == 0){
        return;
    }

    this->theta = this->gyroMagnitude * this->dt; //angle of rotation
    this->angularVelocity.x = this->data->LSM6DSV320_Gyro.getX() / this->gyroMagnitude;
    this->angularVelocity.y = this->data->LSM6DSV320_Gyro.getY() / this->gyroMagnitude;
    this->angularVelocity.z = this->data->LSM6DSV320_Gyro.getZ() / this->gyroMagnitude;

    this->deltaQuaternion.w = cos(this->theta * 0.5); //scalar part
    this->deltaQuaternion.x = this->angularVelocity.x * sin(this->theta * 0.5); //x part
    this->deltaQuaternion.y = this->angularVelocity.y * sin(this->theta * 0.5); //y part
    this->deltaQuaternion.z = this->angularVelocity.z * sin(this->theta * 0.5); //z part

    this->data->orientation.w = this->data->orientation.w * deltaQuaternion.w - this->data->orientation.x * deltaQuaternion.w - this->data->orientation.y * deltaQuaternion.w - this->data->orientation.z * deltaQuaternion.w; // scalar part
    this->data->orientation.x = this->data->orientation.w * deltaQuaternion.x + this->data->orientation.x * deltaQuaternion.x + this->data->orientation.y * deltaQuaternion.x - this->data->orientation.z * deltaQuaternion.x; // x part
    this->data->orientation.y = this->data->orientation.w * deltaQuaternion.y - this->data->orientation.x * deltaQuaternion.y + this->data->orientation.y * deltaQuaternion.y + this->data->orientation.z * deltaQuaternion.y; // y part
    this->data->orientation.z = this->data->orientation.w * deltaQuaternion.z + this->data->orientation.x * deltaQuaternion.z - this->data->orientation.y * deltaQuaternion.z + this->data->orientation.z * deltaQuaternion.z; // z part

    this->data->orientation = this->data->orientation * deltaQuaternion;

    this->data->orientation.normalize();
}
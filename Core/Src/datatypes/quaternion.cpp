#include "datatypes/quaternion.h"

Quaternion::Quaternion() {
    this->w = 1;
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Quaternion::Quaternion(float w, float x, float y, float z) {
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

void Quaternion::normalize() {
    float length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);

    if (length > 0.0f) {
        this->w /= length;
        this->x /= length;
        this->y /= length;
        this->z /= length;
    }
}

/**
 * Multiply this quaternion by another
 *
 * @param q The quaternion to multiply by
 *
 * @return The result of the multiplication
 */
Quaternion Quaternion::operator*(const Quaternion q) {
    Quaternion result;
    result.w = this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z;
    result.x = this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y;
    result.y = this->w * q.y - this->x * q.z + this->y * q.w + this->z * q.x;
    result.z = this->w * q.z + this->x * q.y - this->y * q.x + this->z * q.w;
    return result;
}

/**
 * Multiply this quaternion by another
 *
 * @param q The quaternion to multiply by
 *
 * @return The result of the multiplication
 */
Quaternion Quaternion::operator*=(const Quaternion q) {
    return *this = *this * q;
}

/**
 * Multiply this quaternion by a scalar
 *
 * @param scalar The scalar to multiply by
 *
 * @return The result of the multiplication
 */
Quaternion Quaternion::operator*(float scalar) {
    Quaternion result = *this;
    result.w *= scalar;
    result.x *= scalar;
    result.y *= scalar;
    result.z *= scalar;
    return result;
}

/**
 * Add this quaternion by another
 *
 * @param q The quaternion to add
 *
 * @return The result of the addition
 */
Quaternion Quaternion::operator+(const Quaternion q) {
    Quaternion result;
    result.w = this->w + q.w;
    result.x = this->x + q.x;
    result.y = this->y + q.y;
    result.z = this->z + q.z;
    return result;
}

/**
 * Add this quaternion by another
 *
 * @param q The quaternion to add
 *
 * @return The result of the addition
 */
Quaternion Quaternion::operator+=(const Quaternion q) {
    return *this = *this + q;
}

/**
 * Convert a quaternion to an euler angle
 *
 * @return The euler angle
 */
Vector Quaternion::toEuler() {
    /*
     * https://math.stackexchange.com/q/2975109
     */

    Vector result;

    // Roll
    result.z = atan2f(    2 * (this->w * this->x + this->y * this->z),
                      1 - 2 * (this->x * this->x + this->y * this->y));

    // Pitch
    result.y = 2 * (this->w * this->y + (-this->z) * this->x);
    if (result.y > 1) result.y = 1;
    else if (result.y < -1) result.y = -1;
    result.y = asinf(result.y);

    // Yaw
    result.x = atan2f(    2 * (this->w * this->z + this->x * this->y),
                      1 - 2 * (this->y * this->y + this->z * this->z));


    // Convert to degrees
    result *= 180.0f / M_PI;

    return result;
}

/**
 * Convert an euler angle to a quaternion
 *
 * @param euler The euler angle
 */
void Quaternion::fromEuler(Vector euler) {
    /*
     * Roll is around the Z axis
     * Pitch is around the X axis
     * Yaw is around the Y axis
     *
     * https://math.stackexchange.com/q/2975109
     */

    euler *= M_PI / 180.0f;

    float pitch = euler.y;
    float yaw   = euler.x;
    float roll  = euler.z;

    this->w = cosf(roll / 2) * cosf(pitch / 2) * cosf(yaw / 2) + sinf(roll / 2) * sinf(pitch / 2) * sinf(yaw / 2);
    this->x = sinf(roll / 2) * cosf(pitch / 2) * cosf(yaw / 2) - cosf(roll / 2) * sinf(pitch / 2) * sinf(yaw / 2);
    this->y = cosf(roll / 2) * sinf(pitch / 2) * cosf(yaw / 2) + sinf(roll / 2) * cosf(pitch / 2) * sinf(yaw / 2);
    this->z = cosf(roll / 2) * cosf(pitch / 2) * sinf(yaw / 2) - sinf(roll / 2) * sinf(pitch / 2) * cosf(yaw / 2);
}

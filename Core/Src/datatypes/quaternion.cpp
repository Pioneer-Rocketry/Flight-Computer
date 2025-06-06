#include "datatypes/quaternion.h"

Quaternion::Quaternion() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->w = 1;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion::Quaternion(Vector euler) {
    this->fromEuler(euler);
}

float Quaternion::getX() { return this->x; }
float Quaternion::getY() { return this->y; }
float Quaternion::getZ() { return this->z; }
float Quaternion::getW() { return this->w; }

void Quaternion::setX(float x) { this->x = x; }
void Quaternion::setY(float y) { this->y = y; }
void Quaternion::setZ(float z) { this->z = z; }
void Quaternion::setW(float w) { this->w = w; }

void Quaternion::normalize() {
    float length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
    this->x /= length;
    this->y /= length;
    this->z /= length;
    this->w /= length;
}

/**
 * Multiply this quaternion by another
 *
 * @param q The quaternion to multiply by
 *
 * @return The result of the multiplication
 */
Quaternion Quaternion::operator*(Quaternion q) {
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
Quaternion Quaternion::operator*=(Quaternion q) {
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
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

/**
 * Add this quaternion by another
 *
 * @param q The quaternion to add
 *
 * @return The result of the addition
 */
Quaternion Quaternion::operator+(Quaternion q) {
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
Quaternion Quaternion::operator+=(Quaternion q) {
    return *this = *this + q;
}

/**
 * Convert a quaternion to an euler angle
 *
 * @return The euler angle
 */
Vector Quaternion::toEuler() {
    /*
     * Roll is around the Z axis
     * Pitch is around the X axis
     * Yaw is around the Y axis
     *
     * https://math.stackexchange.com/q/2975109
     */

    float roll = atan2f( 2 * (this->w * this->x + this->y * this->z),
                        1 - 2 * (this->x * this->x + this->y * this->y));
    roll *= 180 / M_PI;

    float pitch = 2 * (this->w * this->y - this->z * this->x);
    if (pitch > 1) pitch = 1;
    else if (pitch < -1) pitch = -1;
    pitch = asinf(pitch);
    pitch *= 180 / M_PI;

    float yaw = atan2f(  2 * (this->w * this->z + this->x * this->y),
                        1 - 2 * (this->y * this->y + this->z * this->z));
    yaw *= 180 / M_PI;

    return Vector(pitch, yaw, roll);
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
    this->x = sinf(euler.getZ() / 2) * cosf(euler.getX() / 2) * cosf(euler.getY() / 2) - cosf(euler.getZ() / 2) * sinf(euler.getX() / 2) * sinf(euler.getY() / 2);
    this->y = cosf(euler.getZ() / 2) * sinf(euler.getX() / 2) * cosf(euler.getY() / 2) + sinf(euler.getZ() / 2) * cosf(euler.getX() / 2) * sinf(euler.getY() / 2);
    this->z = cosf(euler.getZ() / 2) * cosf(euler.getX() / 2) * sinf(euler.getY() / 2) - sinf(euler.getZ() / 2) * sinf(euler.getX() / 2) * cosf(euler.getY() / 2);
    this->w = cosf(euler.getZ() / 2) * cosf(euler.getX() / 2) * cosf(euler.getY() / 2) + sinf(euler.getZ() / 2) * sinf(euler.getX() / 2) * sinf(euler.getY() / 2);
}

/**
 * Convert an axis angle to a quaternion
 *
 * @param axis The axis
 * @param angle The angle
 *
 * @return The quaternion
 */
void Quaternion::fromAxisAngle(Vector axis, float angle) {
    float sinAngle = sinf(angle / 2);

    this->x = axis.getX() * sinAngle;
    this->y = axis.getY() * sinAngle;
    this->z = axis.getZ() * sinAngle;
    this->w = cosf(angle / 2);
}
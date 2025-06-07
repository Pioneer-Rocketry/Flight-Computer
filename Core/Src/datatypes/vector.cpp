#include "datatypes/vector.h"

Vector::Vector() {
    this->zero();
}

Vector::Vector(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector::zero() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Vector Vector::operator+(const Vector v) {
    return Vector(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector Vector::operator+=(const Vector v) {
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    return *this;
}

Vector Vector::operator-(const Vector v) {
    return Vector(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector Vector::operator-=(const Vector v) {
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    return *this;
}

Vector Vector::operator*(const float scalar) {
    return Vector(this->x * scalar, this->y * scalar, this->z * scalar);
}

Vector Vector::operator*=(const float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
}

Vector Vector::operator/(const float scalar) {
    return Vector(this->x / scalar, this->y / scalar, this->z / scalar);
}

Vector Vector::operator/=(const float scalar) {
    this->x /= scalar;
    this->y /= scalar;
    this->z /= scalar;
    return *this;
}

float Vector::dot(Vector v) {
    return this->x * v.x + this->y * v.y + this->z * v.z;
}

Vector Vector::normalize() {
    float length = this->magnitude();
    return Vector(this->x / length, this->y / length, this->z / length);
}

float Vector::magnitude() {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vector Vector::rotateByEuler(const Vector euler) {
    // Calculate sine and cosine values
    float cosPitch = cos(euler.x);
    float sinPitch = sin(euler.x);
    float cosYaw = cos(euler.y);
    float sinYaw = sin(euler.y);
    float cosRoll = cos(euler.z);
    float sinRoll = sin(euler.z);

    Vector result;

    // Apply rotation matrix (ZYX order: Roll * Yaw * Pitch)
    result.x = this->x * (cosYaw * cosRoll) +
               this->y * (sinPitch * sinYaw * cosRoll - cosPitch * sinRoll) +
               this->z * (cosPitch * sinYaw * cosRoll + sinPitch * sinRoll);

    result.y = this->x * (cosYaw * sinRoll) +
               this->y * (sinPitch * sinYaw * sinRoll + cosPitch * cosRoll) +
               this->z * (cosPitch * sinYaw * sinRoll - sinPitch * cosRoll);

    result.z = this->x * (-sinYaw) +
               this->y * (sinPitch * cosYaw) +
               this->z * (cosPitch * cosYaw);

    return result;
}
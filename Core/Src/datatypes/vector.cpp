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
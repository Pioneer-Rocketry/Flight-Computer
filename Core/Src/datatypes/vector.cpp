#include "datatypes/vector.h"

Vector::Vector() {
    this->zero();
}

Vector::Vector(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector::setX(float x) { this->x = x; }
void Vector::setY(float y) { this->y = y; }
void Vector::setZ(float z) { this->z = z; }

float Vector::getX() { return this->x; }
float Vector::getY() { return this->y; }
float Vector::getZ() { return this->z; }

void Vector::zero() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Vector Vector::operator+(Vector v) {
    return Vector(this->x + v.getX(), this->y + v.getY(), this->z + v.getZ());
}

Vector Vector::operator-(Vector v) {
    return Vector(this->x - v.getX(), this->y - v.getY(), this->z - v.getZ());
}

Vector Vector::operator*(float scalar) {
    return Vector(this->x * scalar, this->y * scalar, this->z * scalar);
}

Vector Vector::operator/(float scalar) {
    return Vector(this->x / scalar, this->y / scalar, this->z / scalar);
}

float Vector::dot(Vector v) {
    return this->x * v.getX() + this->y * v.getY() + this->z * v.getZ();
}

Vector Vector::normalize() {
    float length = this->magnitude();
    return Vector(this->x / length, this->y / length, this->z / length);
}

float Vector::magnitude() {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
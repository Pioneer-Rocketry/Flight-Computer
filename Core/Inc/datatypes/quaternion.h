#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h>

#include "datatypes/vector.h"

/**
 * A Quaternion
 */
class Quaternion {
private:

public:
    Quaternion();

    /**
     * Construct a new Quaternion object
     *
     * @param x The x component of the quaternion
     * @param y The y component of the quaternion
     * @param z The z component of the quaternion
     * @param w The w component of the quaternion
     */
    Quaternion(float w, float x, float y, float z);

    void normalize();

    Quaternion operator*(const Quaternion q);
    Quaternion operator*=(const Quaternion q);
    Quaternion operator*(float scalar);

    Quaternion operator+(const Quaternion q);
    Quaternion operator+=(const Quaternion q);

    Vector toEuler();
    void fromEuler(Vector euler);

    float w;
    float x;
    float y;
    float z;
};

#endif /* QUATERNION_H */
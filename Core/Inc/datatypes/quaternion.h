#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h>

#include "datatypes/vector.h"

/**
 * A Quaternion
 */
class Quaternion {
private:
    float x;
    float y;
    float z;
    float w;

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
    Quaternion(float x, float y, float z, float w); 
    Quaternion(Vector euler);

    float getX();
    float getY();
    float getZ();
    float getW();

    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setW(float w);

    void normalize();

    Quaternion operator*(Quaternion q);
    Quaternion operator*=(Quaternion q);

    Vector toEuler();
    void fromEuler(Vector euler);
};

#endif /* QUATERNION_H */
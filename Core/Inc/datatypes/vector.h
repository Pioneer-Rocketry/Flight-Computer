#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

/**
 * A 3D vector
 */
class Vector {
private:

public:
    Vector();

    /**
     * Construct a new Vector object
     *
     * @param x The x component of the vector
     * @param y The y component of the vector
     * @param z The z component of the vector
     */
    Vector(float x, float y, float z);

    void zero();

    Vector operator+ (const Vector v);
    Vector operator+=(const Vector v);
    Vector operator- (const Vector v);
    Vector operator-=(const Vector v);
    Vector operator* (const float scalar);
    Vector operator*=(const float scalar);
    Vector operator/ (const float scalar);
    Vector operator/=(const float scalar);

    float dot(Vector v);
    Vector normalize();
    float magnitude();

    float x;
    float y;
    float z;
};

#endif
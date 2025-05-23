#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

/**
 * A 3D vector
 */
class Vector {
private:
    float x;
    float y;
    float z;

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

    float getX();
    float getY();
    float getZ();

    void setX(float x);
    void setY(float y);
    void setZ(float z);

    void zero();

    Vector operator+(Vector v);
    Vector operator-(Vector v);
    Vector operator*(float scalar);
    Vector operator/(float scalar);

    float dot(Vector v);
    Vector normalize();
    float magnitude();
};

#endif
/*
 * Quaternion.h
 *
 *  Created on: Nov 21, 2025
 *      Author: colin
 *
 * Adapted from https://github.com/peregrine-developments/Orientation/blob/master/Quaternion/Quaternion.h
 *
 * MIT License
 *
 * Copyright (c) 2017 John Carrino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INC_TYPES_QUATERNION_H_
#define INC_TYPES_QUATERNION_H_

#include <math.h>

#include "defines.h"

class Quaternion {
public:
    float a;
    float b;
    float c;
    float d;

    Quaternion() {a = 1; b = c = d = 0;}

    // This is a vector that can be rotated in Quaternion space.
    Quaternion(float x, float y, float z) {a = 0; b = x; c = y; d = z;}

    // ADDED: This is a quaternion
    Quaternion(float x, float y, float z, float g) {a = x; b = y; c = z; d = g;}

    // This returns a Quaternion that rotates in each given axis in radians.
    // We use standard right hand rule for rotations and coordinates.
    static const Quaternion from_euler_rotation(float yaw, float pitch, float roll);

    // This is like from_euler_rotation but for small angles (less than 45 deg (PI/4))
    static const Quaternion from_euler_rotation_approx(float yaw, float pitch, float roll);

    // These should hopefully give better results (less jank)
    static const Quaternion from_axis_angle(float angle, float x, float y, float z);

    Quaternion & operator=(const Quaternion &rhs) {
        a = rhs.a;
        b = rhs.b;
        c = rhs.c;
        d = rhs.d;
        return *this;
    }

    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
    Quaternion & operator*=(const Quaternion &q);
    const Quaternion operator* (const Quaternion& q) const { return Quaternion(*this) *= q; }
    Quaternion & operator+=(const Quaternion &q);
    const Quaternion operator+(const Quaternion& q) const { return Quaternion(*this) += q; }
    Quaternion & operator*=(float scale);
    const Quaternion operator*(float scale) const { return Quaternion(*this) *= scale; }
    float norm() const;
    Quaternion & normalize();
    const Quaternion conj() const;
    // This method takes two vectors and computes the rotation vector between them.
    // Both the left and right hand sides must be pure vectors (a == 0)
    // Both the left and right hand sides must normalized already.
    // This computes the rotation that will tranform this to q.
    const Quaternion rotation_between_vectors(const Quaternion& v) const;
    float dot_product(const Quaternion& q) const;

    // This method takes one vector and rotates it using this Quaternion.
    // The input must be a pure vector (a == 0)
    const Quaternion rotate(const Quaternion& q) const;
    Quaternion & fractional(float f);

    void to_eular(float* roll, float* pitch, float* yaw);
};

#endif /* INC_TYPES_QUATERNION_H_ */

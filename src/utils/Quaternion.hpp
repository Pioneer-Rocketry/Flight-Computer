#pragma once

#include <math.h>

class Quaternion {
    public:

        float w;
        float x;
        float y;
        float z;

        Quaternion() {
            w = 1;
            x = 0;
            y = 0;
            z = 0;
        }

        Quaternion(float w, float x, float y, float z) {
            this->w = w;
            this->x = x;
            this->y = y;
            this->z = z;
        }

        Quaternion(float roll, float pitch, float yaw) {
            // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code
            float cr = cos(roll * 0.5);
            float sr = sin(roll * 0.5);
            float cp = cos(pitch * 0.5);
            float sp = sin(pitch * 0.5);
            float cy = cos(yaw * 0.5);
            float sy = sin(yaw * 0.5);

            w = cr * cp * cy + sr * sp * sy;
            x = sr * cp * cy - cr * sp * sy;
            y = cr * sp * cy + sr * cp * sy;
            z = cr * cp * sy - sr * sp * cy;
        }

        void toEuler(float &roll, float &pitch, float &yaw) {
            // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code_2
            float sinr_cosp = 2 * (w * x + y + z);
            float cosr_cosp = 1 - 2 * (x * x + y * y);
            roll = atan2f(sinr_cosp, cosr_cosp);
            
            float sinp = sqrt(1 + 2 * (w * y - x * z));
            float cosp = sqrt(1 - 2 * (w * y - x * z));
            pitch = 2 * atan2(sinp, cosp) - M_PI / 2;

            float siny_cosp = 2 * (w * z + x * y);
            float cosy_cosp = 1 - 2  * (y  * y  + z  * z );
            yaw = atan2(siny_cosp, cosy_cosp);

        }

};
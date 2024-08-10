#pragma once

#include <math.h>
#include <BasicLinearAlgebra.h>

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
			// Convert degrees to radians
			roll  *= M_PI / 180.0;
			pitch *= M_PI / 180.0;
			yaw   *= M_PI / 180.0;

			// Convert Euler angles (in radians) to a quaternion
			float cy = cos(yaw * 0.5);
			float sy = sin(yaw * 0.5);
			float cp = cos(roll * 0.5);
			float sp = sin(roll * 0.5);
			float cr = cos(pitch * 0.5);
			float sr = sin(pitch * 0.5);

			w = cr * cp * cy + sr * sp * sy;
			x = sr * cp * cy - cr * sp * sy;
			y = cr * sp * cy + sr * cp * sy;
			z = cr * cp * sy - sr * sp * cy;
		}

		void toEuler(float &roll, float &pitch, float &yaw) {
			// Convert a quaternion to Euler angles (in radians)
			// roll (x-axis rotation)
			float sinr_cosp = 2 * (w * x + y * z);
			float cosr_cosp = 1 - 2 * (x * x + y * y);
			pitch = atan2(sinr_cosp, cosr_cosp);

			// pitch (y-axis rotation)
			float sinp = 2 * (w * y - z * x);
			if (fabs(sinp) >= 1)
				roll = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
			else
				roll = asin(sinp);

			// yaw (z-axis rotation)
			float siny_cosp = 2 * (w * z + x * y);
			float cosy_cosp = 1 - 2 * (y * y + z * z);
			yaw = atan2(siny_cosp, cosy_cosp);

			// Convert radians to degrees
			roll  *= 180.0 / M_PI;
			pitch *= 180.0 / M_PI;
			yaw   *= 180.0 / M_PI;
		}

		BLA::Matrix<4, 1> getMatrix() {
			BLA::Matrix<4, 1> tmp = {
				w, x, y, z
			};

			return tmp;
		}
};

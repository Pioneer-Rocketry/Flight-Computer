// utils/Types.h
#ifndef TYPES_h
#define TYPES_h

#include <Arduino.h>

class dataPoint {
	private:
		union {
			float value;
			byte array[4];
		} dataT;

	public:
		operator float() const {
			return dataT.value;
		}

		dataPoint operator+=(const float value) {
			dataT.value += value;
			return *this;
		}

		byte* getArray() {
			return dataT.array;
		}

		float operator=(float value) {
			dataT.value = value;
			return *this;
		}
};

struct Vector
{
	dataPoint X;
	dataPoint Y;
	dataPoint Z;
};

struct GPS_data {
    dataPoint time;
    dataPoint lat;
    dataPoint lng;
    dataPoint sat;
    dataPoint alt;
    dataPoint speed;
    dataPoint hdop;
};

struct Pyro_data {
    bool continuity;
    bool fired;
};

struct Kalman_data {
    Vector velocity;
    Vector position;
};

#endif // TYPES_h
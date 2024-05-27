// Datapoint.h
#ifndef DATAPOINT_h
#define DATAPOINT_h

#include <Arduino.h>

#include "utils/Quaternion.hpp"

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

const int size = 38;
class Data {
	public:
		dataPoint time;

		dataPoint accX;
		dataPoint accY;
		dataPoint accZ;
		
		dataPoint gyrX;
		dataPoint gyrY;
		dataPoint gyrZ;

		dataPoint magX;
		dataPoint magY;
		dataPoint magZ;
		dataPoint heading;

		Quaternion quat;

		Vector attitude;
		Vector position;
		Vector velocity;
		Vector angular_rate;

		dataPoint temp;
		dataPoint press;
		dataPoint alt;
		
		dataPoint filted_alt;
		dataPoint starting_alt;

		dataPoint state;

		dataPoint gps_lat;
		dataPoint gps_lng;
		dataPoint gps_sat;
		dataPoint gps_alt;
		dataPoint gps_speed;
		dataPoint gps_hdop;

		dataPoint air_speed;

		dataPoint pyro_arm;

		dataPoint pyro_1_con;
		dataPoint pyro_2_con;
		dataPoint pyro_3_con;
		dataPoint pyro_4_con;

		dataPoint pyro_1_fired;
		dataPoint pyro_2_fired;
		dataPoint pyro_3_fired;
		dataPoint pyro_4_fired;

		dataPoint allowStage;
		dataPoint dt;

		dataPoint array[size];

		String get_data() {
			String data = "";

			data += String(time, 3) + ",";

			// data += log(accX, 5);
			// data += log(accY, 5);
			// data += log(accZ, 5);

			// data += log(gyrX, 5);
			// data += log(gyrY, 5);
			// data += log(gyrZ, 5);

			// data += log(magX, 2);
			// data += log(magY, 2);
			// data += log(magZ, 2);
			// data += log(heading, 2);
			
			data += log(attitude.X, 1);
			data += log(attitude.Y, 1);
			data += log(attitude.Z, 1);
			
			// data += log(position.X, 1);
			// data += log(position.Y, 1);
			// data += log(position.Z, 1);
			
			// data += log(velocity.X, 1);
			// data += log(velocity.Y, 1);
			// data += log(velocity.Z, 1);
			
			// data += log(angular_rate.X, 1);
			// data += log(angular_rate.Y, 1);
			// data += log(angular_rate.Z, 1);

			// data += log(temp,  2);
			// data += log(press, 2);
			// data += log(alt,   2);
			// data += log(filted_alt, 2);

			// data += log(state, 0);

			// data += log(gps_lat,   8);
			// data += log(gps_lng,   8);
			// data += log(gps_sat,   0);
			// data += log(gps_alt,   2);
			// data += log(gps_speed, 2);
			// data += log(gps_hdop,  2);

			// data += log(pyro_arm, 0);

			// data += log(pyro_1_con, 0);
			// data += log(pyro_2_con, 0);
			// data += log(pyro_3_con, 0);
			// data += log(pyro_4_con, 0);

			// data += log(pyro_1_fired, 0);
			// data += log(pyro_2_fired, 0);
			// data += log(pyro_3_fired, 0);
			// data += log(pyro_4_fired, 0);

			return data;
		}

		String log(dataPoint data, int floatingPoint=2) {
			String tmp = String(data, floatingPoint) + ",";
			// if (data.value > 0) tmp = " " + tmp;
			
			return tmp;
		}

		Data() {
			allowStage = 0;
		}
};

#endif
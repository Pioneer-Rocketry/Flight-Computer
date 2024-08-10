// Datapoint.h
#ifndef DATAPOINT_h
#define DATAPOINT_h

#include <Arduino.h>

#include "utils/Quaternion.hpp"
#include "utils/Types.hpp"

const int size = 38;
class Data {
	public:
		dataPoint time;

		bool newImuData;
		bool newBaroData;
		bool newGPSdata;
		
		Vector acc;
		Vector gyro;
		Vector mag;
		dataPoint heading;

		Quaternion quat;

		dataPoint temp;
		dataPoint press;
		dataPoint alt;
		dataPoint starting_alt;

		Kalman_data kalman;

		dataPoint state;

		GPS_data gps;
		dataPoint starting_gps_alt;

		dataPoint air_speed;

		bool pyro_arm;
		bool allowStaging;
		Pyro_data pyro1;
		Pyro_data pyro2;
		Pyro_data pyro3;
		Pyro_data pyro4;

		dataPoint dt;

		dataPoint p1;
		dataPoint p2;
		dataPoint p3;
		dataPoint p4;

		dataPoint array[size];

		String get_data() {
			String data = "";

			data += String(time, 3) + ",";

			// data += log(accX, 2);
			// data += log(accY, 2);
			// data += log(accZ, 2);

			// data += log(gyrX, 2);
			// data += log(gyrY, 2);
			// data += log(gyrZ, 2);

			// data += log(magX, 2);
			// data += log(magY, 2);
			// data += log(magZ, 2);
			// data += log(heading, 2);
			
			// data += log(pitch, 1);
			// data += log(roll, 1);
			// data += log(yaw, 1);
			
			// data += log(imuPitch, 1);
			// data += log(imuRoll, 1);
			// data += log(imuYaw, 1);
			
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
			// data += log(alt,   1);

			// data += log(state, 0);

			// data += log(gps_lat,   8);
			// data += log(gps_lng,   8);
			// data += log(gps_sat,   0);
			// data += log(gps_alt,   1);
			// data += log(gps_speed, 2);
			// data += log(gps_hdop,  2);
			// data += String(newGPSdata) + ",";
			// data += log(starting_gps_alt,   1);

			// data += log(filted_alt, 1);
			// data += log(verticalVelocity, 2);
			
			// data += log(pyro_arm, 0);

			// data += log(pyro_1_con, 0);
			// data += log(pyro_2_con, 0);
			// data += log(pyro_3_con, 0);
			// data += log(pyro_4_con, 0);

			// data += log(pyro_1_fired, 0);
			// data += log(pyro_2_fired, 0);
			// data += log(pyro_3_fired, 0);
			// data += log(pyro_4_fired, 0);

			// data += log(p1, 3);
			// data += log(p2, 3);
			// data += log(p3, 3);
			// data += log(p4, 3);

			return data;
		}

		String log(dataPoint data, int floatingPoint=2) {
			String tmp = String(data, floatingPoint) + ",";
			// if (data > 0) tmp = " " + tmp;
			
			return tmp;
		}

		Data() {
			allowStaging = 0;
			starting_gps_alt = -999;
		}
};

#endif
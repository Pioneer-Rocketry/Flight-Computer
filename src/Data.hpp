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

		dataPoint array[size];

		String get_data() {
			String data = "";

			data += String(time, 3) + ",";

			// data += log(acc.X, 2);
			// data += log(acc.Y, 2);
			// data += log(acc.Z, 2);

			// data += log(gyro.X, 2);
			// data += log(gyro.Y, 2);
			// data += log(gyro.Z, 2);

			// data += log(mag.X, 2);
			// data += log(mag.Y, 2);
			// data += log(mag.Z, 2);
			// data += log(heading, 2);
			
			// data += log(kalman.position.X, 1);
			// data += log(kalman.position.Y, 1);
			// data += log(kalman.position.Z, 1);
			
			// data += log(kalman.velocity.X, 1);
			// data += log(kalman.velocity.Y, 1);
			// data += log(kalman.velocity.Z, 1);
			
			// data += log(temp,  2);
			// data += log(press, 2);
			// data += log(alt,   1);

			// data += log(state, 0);

			// data += log(gps.lat,   8);
			// data += log(gps.lng,   8);
			// data += log(gps.sat,   0);
			// data += log(gps.alt,   1);
			// data += log(gps.speed, 2);
			// data += log(gps.hdop,  2);
			// data += log(newGPSdata);
			// data += log(starting_gps_alt,   1);

			// data += log(pyro_arm);

			// data += log(pyro1.continuity);
			// data += log(pyro2.continuity);
			// data += log(pyro3.continuity);
			// data += log(pyro4.continuity);

			// data += log(pyro1.fired);
			// data += log(pyro2.fired);
			// data += log(pyro3.fired);
			// data += log(pyro4.fired);

			return data;
		}

		String log(dataPoint data, int floatingPoint=2) {
			String tmp = String(data, floatingPoint) + ",";
			// if (data > 0) tmp = " " + tmp;
			
			return tmp;
		}

		String log(bool data) {
			return data + ",";
		}

		Data() {
			allowStaging = 0;
			starting_gps_alt = -999;
		}
};

#endif
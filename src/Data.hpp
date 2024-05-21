// Datapoint.h
#ifndef DATAPOINT_h
#define DATAPOINT_h

#include <Arduino.h>

#include "utils/Quaternion.hpp"

typedef union
{
  float value;
  byte array[4];
}
dataPointT;

const int size = 38;
class Data {
	public:
		dataPointT time;

		dataPointT accX;
		dataPointT accY;
		dataPointT accZ;
		
		dataPointT gyrX;
		dataPointT gyrY;
		dataPointT gyrZ;

		dataPointT magX;
		dataPointT magY;
		dataPointT magZ;
		dataPointT heading;

		Quaternion quat;

		dataPointT roll;
		dataPointT pitch;
		dataPointT yaw;

		dataPointT P1;
		dataPointT P2;
		dataPointT P3;
		dataPointT P4;

		dataPointT Pp1;
		dataPointT Pp2;
		dataPointT Pp3;
		dataPointT Pp4;

		dataPointT velX;
		dataPointT velY;
		dataPointT velZ;

		dataPointT temp;
		dataPointT press;
		dataPointT alt;
		
		dataPointT filted_alt;
		dataPointT starting_alt;

		dataPointT state;

		dataPointT gps_lat;
		dataPointT gps_lng;
		dataPointT gps_sat;
		dataPointT gps_alt;
		dataPointT gps_speed;
		dataPointT gps_hdop;

		dataPointT air_speed;

		dataPointT pyro_arm;

		dataPointT pyro_1_con;
		dataPointT pyro_2_con;
		dataPointT pyro_3_con;
		dataPointT pyro_4_con;

		dataPointT pyro_1_fired;
		dataPointT pyro_2_fired;
		dataPointT pyro_3_fired;
		dataPointT pyro_4_fired;

		dataPointT allowStage;
		dataPointT dt;

		dataPointT array[size];

		String get_data() {
			String data = "";

			data += String(Data::time.value, 3) + ",";

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

			quat.toEuler(roll.value, pitch.value, yaw.value);
			data += log(pitch, 1);
			// data += log(roll,  1);
			// data += log(yaw,   1);
			
			// data += log(P1, 4);
			// data += log(P2, 4);
			// data += log(P3, 4);
			// data += log(P4, 4);

			// data += log(Pp1, 4);
			// data += log(Pp2, 4);
			// data += log(Pp3, 4);
			// data += log(Pp4, 4);

			// data += log(velX, 2);
			// data += log(velY, 2);
			// data += log(velZ, 2);

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

		String log(dataPointT data, int floatingPoint=2) {
			String tmp = String(data.value, floatingPoint) + ",";
			// if (data.value > 0) tmp = " " + tmp;
			
			return tmp;
		}

		Data() {
			allowStage.value = 0;
		}
};

#endif
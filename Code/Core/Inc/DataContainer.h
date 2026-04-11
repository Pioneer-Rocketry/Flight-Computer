/*
 * DataContainer.h
 *
 *  Created on: Oct 26, 2025
 *      Author: colin
 */

#ifndef INC_DATACONTAINER_H_
#define INC_DATACONTAINER_H_

#include <cstdint>

/**
 * @class DataContainer
 * @brief Central data storage and communication hub for all subsystems.
 *
 * The DataContainer class provides a unified interface for storing and
 * accessing shared data across different subsystems such as navigation,
 * sensors, and control. It acts as the primary communication medium between
 * modules, reducing direct coupling and improving system modularity.
 *
 * Typical responsibilities include:
 *  - Holding sensor measurements (IMU, GPS, barometer, etc.)
 *  - Storing derived state variables (attitude, velocity, position)
 *  - Providing thread-safe access or update methods (if applicable)
 *  - Serving as a common data reference for Subsystem and Device classes
 *
 * This class is designed to be passed by pointer or reference to other
 * subsystems during initialization.
 */
class DataContainer {
private:

public:
	/* Guidance Data */
	float p;
	float i;
	float d;

	float error;
	float lastError;
	float PID;

	float guidanceDt;

	float target;

	float flightTime;
	float launchTime;

	/* Navigation Data */
	/* IMU Data*/
	float LSM6DSV320GyroX_dps;
	float LSM6DSV320GyroY_dps;
	float LSM6DSV320GyroZ_dps;

	float LSM6DSV320LowGAccelX_mps2;
	float LSM6DSV320LowGAccelY_mps2;
	float LSM6DSV320LowGAccelZ_mps2;

	float LSM6DSV320HighGAccelX_mps2;
	float LSM6DSV320HighGAccelY_mps2;
	float LSM6DSV320HighGAccelZ_mps2;

	/* Barometer Data*/
	float MS560702BA03Temperature_C;
	float MS560702BA03Pressure_hPA;
	float MS560702BA03Altitude_m;

	float startingBaroAltitude_m;
	float baroAltitudeOffset_m;

	/* GPS Data */
	// Latitude in decimal degrees (+N, -S)
	float GPSLatitude;
	// Longitude in decimal degrees (+E, -W)
	float GPSLongitude;
	// Altitude in meters (from GPS)
	float GPSAltitude_m;
	// GPS fix status: 0 = invalid, 1 = GPS fix, 2 = DGPS
	int GPSFix;
	// Number of satellites used in fix
	int GPSNumSatellites;
	// UTC time string from GPS (hhmmss.sss)
	char GPSUTCTime[16];

	/* Quaternion Data */
	float quaternionW;
	float quaternionX;
	float quaternionY;
	float quaternionZ;

	float quaternionNorm;

	/* Euler Angles */
	float roll;
	float pitch;
	float yaw;

	float intergratedRoll;

	/* Kalman Filter Data */
	float KalmanFilterAltitude_m;
	float KalmanFilterVerticalVelocity_mps;
	float KalmanFilterVerticalAcceleration_mps2;

	/* Control Data */
	int16_t servo1Angle;
    int16_t servo2Angle;
    int16_t servo3Angle;
    int16_t servo4Angle;

};



#endif /* INC_DATACONTAINER_H_ */

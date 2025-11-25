/*
 * DataContainer.h
 *
 *  Created on: Oct 26, 2025
 *      Author: colin
 */

#ifndef INC_DATACONTAINER_H_
#define INC_DATACONTAINER_H_

#include <cstdint>
#include <Eigen/Geometry>

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
	Eigen::Quaternionf orientation_quat;
	Eigen::Vector3f orientation_eular;

	float roll;
	float pitch;
	float yaw;

	/* IMU Data*/
	float LSM6DSV320GyroX;
	float LSM6DSV320GyroY;
	float LSM6DSV320GyroZ;

	float LSM6DSV320LowGAccelX;
	float LSM6DSV320LowGAccelY;
	float LSM6DSV320LowGAccelZ;

	float LSM6DSV320HighGAccelX;
	float LSM6DSV320HighGAccelY;
	float LSM6DSV320HighGAccelZ;

	/* Barometer Data*/
	float MS560702BA03Temperature;
	float MS560702BA03Pressure;
	float MS560702BA03Altitude;

	/* GPS Data */
	// Latitude in decimal degrees (+N, -S)
	float GPSLatitude;
	// Longitude in decimal degrees (+E, -W)
	float GPSLongitude;
	// Altitude in meters (from GPS)
	float GPSAltitude;
	// GPS fix status: 0 = invalid, 1 = GPS fix, 2 = DGPS
	int GPSFix;
	// Number of satellites used in fix
	int GPSNumSatellites;
	// UTC time string from GPS (hhmmss.sss)
	char GPSUTCTime[16];


	/* Kalman Filter Data */
	float KalmanFilterPositionX;
	float KalmanFilterPositionY;
	float KalmanFilterPositionZ;

	float KalmanFilterVelocityX;
	float KalmanFilterVelocityY;
	float KalmanFilterVelocityZ;

	float KalmanFilterAccelerationX;
	float KalmanFilterAccelerationY;
	float KalmanFilterAccelerationZ;
};



#endif /* INC_DATACONTAINER_H_ */

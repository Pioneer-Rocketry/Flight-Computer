/*
 * Navigation.h
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_NAVIGATION_H_
#define SRC_SUBSYSTEMS_NAVIGATION_H_

#include <Eigen/Dense>

#include "Subsystem.h"
#include "main.h"

#include "Devices/SPI_Devices/LSM6DSV320.h"
#include "Devices/SPI_Devices/MS560702BA03.h"
#include "Devices/GPS.h"

#include "Types/Quaternion.h"

#include "defines.h"

/**
 * @class Navigation
 * @brief Handles flight navigation logic and sensor data fusion.
 *
 * The Navigation subsystem is responsible for processing and combining data
 * from various sensors (e.g., IMU, barometer, magnetometer) to estimate
 * the vehicle’s orientation, position, and velocity in real-time.
 *
 * This class inherits from the base `Subsystem` interface and implements
 * the required `init()` and `update()` methods to manage its setup and
 * periodic updates within the main control loop.
 *
 * Typical tasks include:
 *  - Initializing navigation sensors
 *  - Running sensor fusion algorithms (e.g., Kalman or complementary filter)
 *  - Updating the main DataContainer with current state estimates
 */
class Navigation: public Subsystem {
public:
	/**
	 * @brief Navigation subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 * @param spiBus Reference to the SPI Bus that is connected to the sensors
	 * @param uart Refrences the UART that is connected to the GPS
	 */
	Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart);

	/**
	 * @brief Initializes the navigation subsystem.
	 *
	 * This method sets up required sensors, allocates necessary buffers,
	 * and prepares the system for navigation updates.
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int init() override;

	/**
	 * @brief Updates navigation data and state estimation.
	 *
	 * This function should be called periodically (e.g., at a fixed update rate)
	 * to process new sensor data, compute navigation estimates, and store
	 * the results in the shared DataContainer.
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;

private:
	// Sensors
	LSM6DSV320 imu;		// LSM6DSV320 IMU for measuring Low-G Accelerations, High-G Accelerations, and Angular Rates
	MS560702BA03 baro;	// MS560702BA03 Barometer for measuring Barometric Altitude, and Temperature
	GPS gps;			// GPR for Absolute Positioning: Latitude, Longitude, Altitude

	// Orientation Tracking
	float norm;				// Normal of the Roll, Pitch, Yaw Rates
	float rollRate_rad;		// Roll Rate in radians/sec
	float pitchRate_rad;	// Pitch Rate in radians/sec
	float yawRate_rad;		// Yaw Rate in radians/sec

	// Kalman Filter
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_STATES, 		1> 									x;  // State vector
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_MEASUREMENTS, 1> 									Z;  // Measurements
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_STATES, 		KALMAN_FILTER_NUM_OF_STATES> 		F;  // State transition matrix
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_MEASUREMENTS, KALMAN_FILTER_NUM_OF_STATES> 		H;  // Measurement matrix
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_STATES, 		KALMAN_FILTER_NUM_OF_STATES> 		Q;  // Process noise covariance
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_MEASUREMENTS, KALMAN_FILTER_NUM_OF_MEASUREMENTS> 	R;  // Measurement noise covariance
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_STATES, 		KALMAN_FILTER_NUM_OF_STATES> 		P;  // Estimate error covariance
	Eigen::Matrix<float, KALMAN_FILTER_NUM_OF_STATES, 		KALMAN_FILTER_NUM_OF_MEASUREMENTS> 	K;  // Kalman gain

	/**
	 * @brief Initializes the Kalman Filter
	 *
	 * This function setups up the necessary matrices and variables for the Kalman Filter.
	 */
	void initKalmanFilter();
	bool isKalmanFilterInit = false;

	// Timing
	float lastLoop;	// Time of start of previous loop in ms
	float freq;		// Frequency of the loop = 1/dt
	float now;		// Current time in ms
	float dt;		// Time since last loop in ms
	float dt2;		// dt^2/2 for kalman filter

};

#endif /* SRC_SUBSYSTEMS_NAVIGATION_H_ */

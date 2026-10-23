/*
 * Navigation.h
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_NAVIGATION_H_
#define SRC_SUBSYSTEMS_NAVIGATION_H_

#include "Subsystem.h"
#include "main.h"

#include "Devices/SPI_Devices/LSM6DSV320.h"
#include "Devices/SPI_Devices/MS560702BA03.h"
#include "Devices/GPS.h"

#include "matrix.hpp"
#include "utils.h"
#include "defines.h"

#define GYRO_THRESHOLD_DPS 0.1f

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
	Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart, uint8_t* gpsRxBuffer);

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
	float quaternionWDot;
	float quaternionXDot;
	float quaternionYDot;
	float quaternionZDot;

	float rollRate_rad;
	float pitchRate_rad;
	float yawRate_rad;

	float siny_cosp;
	float cosy_cosp;
	float sinp;
	float sinr_cosp;
	float cosr_cosp;

	void integrateQuaternion();
	void initializeQuaternion();

	void rotateVectorByQuaternion(Matrix<float, 3, 1>& vec);
	float lowGVarianceScale() const;

	// Kalman Filter
	Matrix<float, 3, 1> lowG;
	Matrix<float, 3, 1> highG;

	static constexpr int AXIS_COUNT = 3;
	static constexpr int AXIS_STATE_COUNT = 3;
	enum AxisStateIndex { POSITION = 0, VELOCITY = 1, ACCELERATION = 2 };

	// Independent [position, velocity, acceleration] filters for X, Y, and Z.
	Matrix<float, AXIS_STATE_COUNT, 1> axisState[AXIS_COUNT];
	Matrix<float, AXIS_STATE_COUNT, AXIS_STATE_COUNT> axisCovariance[AXIS_COUNT];
	Matrix<float, AXIS_STATE_COUNT, AXIS_STATE_COUNT> F;
	Matrix<float, AXIS_STATE_COUNT, AXIS_STATE_COUNT> Q;
	Matrix<float, AXIS_STATE_COUNT, AXIS_STATE_COUNT> I;

	float processNoise = 0.01f;

	// Provisional values; replace with variances measured from flight hardware.
	float lowGNoise		= 0.5f;
	float highGNoise	= 2.0f;
	float baroNoise 	= 3.0f;

	/**
	 * @brief Initializes the Kalman Filter
	 *
	 * This function setups up the necessary matrices and variables for the Kalman Filter.
	 */
	void initKalmanFilter();

	void updateKalmanFilter();

	void runKalmanFilter();
	void scalarMeasurementUpdate(int axis, AxisStateIndex stateIndex,
							 float measurement, float variance);

	bool isKalmanFilterInit = false;

	// Timing
	uint32_t lastLoop_us;	// Time of start of previous loop in microseconds
	float freq;		// Frequency of the loop = 1/dt
	uint32_t now_us;	// Current time in microseconds
	float dt_s;		// Time since last loop in s
	float dt2, dt3, dt4;	// dt^2, dt^3, dt^4
};

#endif /* SRC_SUBSYSTEMS_NAVIGATION_H_ */

/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart)
	: Subsystem(data),
	  imu(data, spiBus, SPI_CS1_GPIO_Port, SPI_CS1_Pin),
	  baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
	  gps(data, uart)
{
	data->KalmanFilterPositionX = 0.0f;
	data->KalmanFilterPositionY = 0.0f;
	data->KalmanFilterPositionZ = 0.0f;

	data->KalmanFilterVelocityX = 0.0f;
	data->KalmanFilterVelocityY = 0.0f;
	data->KalmanFilterVelocityZ = 0.0f;
}


int Navigation::init()
{
	data->roll = 0.0f;
	data->pitch = 0.0f;
	data->yaw = 0.0f;

	if (imu.deviceInit() < 0)
	{
		return -1;
	}

	if (baro.deviceInit() < 0)
	{
		return -1;
	}

	if (gps.deviceInit() < 0)
	{
		return -1;
	}

	return 0;
}


int Navigation::update()
{
	now = HAL_GetTick();
	dt = now - lastLoop;
	freq = 1000.0f / dt;
	lastLoop = now;

	// -------------------------------------------------------------
	// Read Sensor Data
	// -------------------------------------------------------------

	imu.updateDevice();

	baro.updateDevice();

	// -------------------------------------------------------------
	// Quaterion Intergration
	// Adapted from https://github.com/peregrine-developments/Orientation/blob/master/Orientation/Orientation.cpp
	// -------------------------------------------------------------
	rollRate_rad	= data->LSM6DSV320GyroY * DEG_TO_RAD;
	pitchRate_rad 	= data->LSM6DSV320GyroX * DEG_TO_RAD;
	yawRate_rad 	= data->LSM6DSV320GyroZ * DEG_TO_RAD;

	norm = sqrtf(powf(pitchRate_rad, 2) + powf(rollRate_rad, 2) + powf(yawRate_rad, 2));
	norm = copysignf(fmax(abs(norm), 1e-9), norm);

	data->orientation *= Quaternion::from_axis_angle(dt * norm, rollRate_rad / norm, pitchRate_rad / norm, yawRate_rad / norm);
	data->orientation.to_eular(&data->roll, &data->pitch, &data->yaw);

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	if (!isKalmanFilterInit)
		initKalmanFilter();

	// Update Measurements
	Z(0) = data->LSM6DSV320LowGAccelX;
	Z(1) = data->LSM6DSV320LowGAccelY;
	Z(2) = data->LSM6DSV320LowGAccelZ;
	Z(3) = data->LSM6DSV320HighGAccelX;
	Z(4) = data->LSM6DSV320HighGAccelY;
	Z(5) = data->LSM6DSV320HighGAccelZ;
	Z(6) = data->MS560702BA03Altitude;

	// Predict State
	x = F * x;

	// Predict Error Covariance
	P = F * P * F.transpose() + Q;

	// Compute Kalman Gain
	K = P * H.transpose() * (H * P * H.transpose() + R).inverse();

	// Compute the Estimate
	x = x + K * (Z - H * x);

	// Computer the Error covariance
	P = P - K * H * P;

	data->KalmanFilterPositionX = x(0);
	data->KalmanFilterPositionY = x(1);
	data->KalmanFilterPositionZ = x(2);
	data->KalmanFilterVelocityX = x(3);
	data->KalmanFilterVelocityY = x(4);
	data->KalmanFilterVelocityZ = x(5);

	return 0;
}

void Navigation::initKalmanFilter()
{

	// Initialize Kalman Filter Matricies

	// Initial State
	x.setZero();

	// State Transition

	// Measurement
	H.setZero();
	H(0,0) = 0.0f; // Set initial Low G Acceleration X to 0
	H(1,1) = 0.0f; // Set initial Low G Acceleration Y to 0
	H(2,2) = 0.0f; // Set initial Low G Acceleration Z to 0
	H(3,3) = 0.0f; // Set initial High G Acceleration X to 0
	H(4,4) = 0.0f; // Set initial High G Acceleration Y to 0
	H(5,5) = 0.0f; // Set initial High G Acceleration Z to 0
	H(6,6) = data->MS560702BA03Altitude; // Set the initial Barometric Altitude to the current altitude

	// Process Noise

	// Measurement Nose

	// Estimate Error
	P.setIdentity();
	P *= 1.0;  // initial uncertainty

	isKalmanFilterInit = true;
}

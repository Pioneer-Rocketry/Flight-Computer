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

	data->KalmanFilterAccelerationX = 0.0f;
	data->KalmanFilterAccelerationY = 0.0f;
	data->KalmanFilterAccelerationZ = 0.0f;
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

	updateKalmanFilter();

	runKalmanFilter();

	data->KalmanFilterPositionX 	= x(0);
	data->KalmanFilterAccelerationX = x(1);
	data->KalmanFilterVelocityX 	= x(2);

	data->KalmanFilterPositionY 	= x(3);
	data->KalmanFilterAccelerationY = x(4);
	data->KalmanFilterVelocityY 	= x(5);

	data->KalmanFilterPositionZ 	= x(6);
	data->KalmanFilterAccelerationZ = x(7);
	data->KalmanFilterVelocityZ 	= x(8);

	return 0;
}

void Navigation::initKalmanFilter()
{
	// Initialize Kalman Filter Matricies

	// Initial State
	x.setZero();
	x(1) = data->LSM6DSV320LowGAccelX;
	x(4) = data->LSM6DSV320LowGAccelY;
	x(6) = data->MS560702BA03Altitude; // Set the initial Barometric Altitude to the current altitude
	x(8) = data->LSM6DSV320LowGAccelZ;

	// State Transition
	F.setIdentity();
	F(0, 1) = F(1, 2) = F(3, 4) = F(4, 5) = F(6, 7) = F(7, 8) = NAVIGATION_TARGET_DT;
	F(0, 2) = F(3, 5) = F(7, 8) = (NAVIGATION_TARGET_DT * NAVIGATION_TARGET_DT) / 2;

	// Get Initial Measurements
	Z.setZero();
	Z(0) = data->LSM6DSV320LowGAccelX;
	Z(1) = data->LSM6DSV320LowGAccelY;
	Z(2) = data->LSM6DSV320LowGAccelZ;
	Z(3) = data->LSM6DSV320HighGAccelX;
	Z(4) = data->LSM6DSV320HighGAccelY;
	Z(5) = data->LSM6DSV320HighGAccelZ;
	Z(6) = data->MS560702BA03Altitude;

	// Observation
	H.setZero();
	H(0, 2) = H(1, 5) = H(2, 8) = H(3, 2) = H(4, 5) = H(5, 8) = H(6, 6) = 1;

	// Process Noise
	Q.setZero();
	Q(0, 0) = Q(3, 3) = Q(6, 6) = powf(NAVIGATION_TARGET_DT, 4)/4.0f;
	Q(0, 1) = Q(1, 0) = Q(3, 4) = Q(4, 3) = Q(6, 7) = Q(7, 6) = powf(NAVIGATION_TARGET_DT, 3)/2.0f;
	Q(0, 2) = Q(2, 0) = Q(3, 5) = Q(5, 3) = Q(6, 8) = Q(8, 6) = powf(NAVIGATION_TARGET_DT, 2)/2.0f;
	Q(1, 1) = Q(4, 4) = Q(7, 7) = powf(NAVIGATION_TARGET_DT, 2);
	Q(1, 2) = Q(2, 1) = Q(4, 5) = Q(5, 4) = Q(7, 8) = Q(8, 7) =NAVIGATION_TARGET_DT;

	Q *= processNoise;

	// Measurement Noise
	R.setIdentity();
	R(0, 0) = R(1, 1) = R(2, 2) = powf(lowGNoise, 2);
	R(3, 3) = R(4, 4) = R(5, 5) = powf(highGNoise, 2);
	R(6, 6) = powf(baroNoise, 2);

	// Estimate Error
	P.setIdentity();
	P *= 1.0;  // initial uncertainty

	isKalmanFilterInit = true;

	return;
}

void Navigation::updateKalmanFilter()
{
	// Update Measurements
	Z(0) = data->LSM6DSV320LowGAccelX;
	Z(1) = data->LSM6DSV320LowGAccelY;
	Z(2) = data->LSM6DSV320LowGAccelZ;
	Z(3) = data->LSM6DSV320HighGAccelX;
	Z(4) = data->LSM6DSV320HighGAccelY;
	Z(5) = data->LSM6DSV320HighGAccelZ;
	Z(6) = data->MS560702BA03Altitude;

	// Update State Transition Matrix
	F(0, 1) = F(1, 2) = F(3, 4) = F(4, 5) = F(6, 7) = F(7, 8) = dt;
	F(0, 2) = F(3, 5) = F(7, 8) = powf(dt, 2) / 2.0f;

	// Update Process Noise
	Q.setZero();
	Q(0, 0) = Q(3, 3) = Q(6, 6) = powf(dt, 4)/4.0f;
	Q(0, 1) = Q(1, 0) = Q(3, 4) = Q(4, 3) = Q(6, 7) = Q(7, 6) = powf(dt, 3)/2.0f;
	Q(0, 2) = Q(2, 0) = Q(3, 5) = Q(5, 3) = Q(6, 8) = Q(8, 6) = powf(dt, 2)/2.0f;
	Q(1, 1) = Q(4, 4) = Q(7, 7) = powf(dt, 2);
	Q(1, 2) = Q(2, 1) = Q(4, 5) = Q(5, 4) = Q(7, 8) = Q(8, 7) = dt;

	Q *= processNoise;

	runKalmanFilter();
}

void Navigation::runKalmanFilter()
{
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
}

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

	return 0;
}

/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

#include "main.h"

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	  accl(data, spiBus, IMU_CS_GPIO_Port, IMU_CS_Pin)
{

}


int Navigation::init()
{
	if (accl.deviceInit() < 0)
	{
		return -1;
	}

	return 0;
}


int Navigation::update()
{
	// -------------------------------------------------------------
	// Read Sensor Data
	// -------------------------------------------------------------

	accl.updateDevice();

	// -------------------------------------------------------------
	// Quaterion Intergration
	// -------------------------------------------------------------

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	return 0;
}

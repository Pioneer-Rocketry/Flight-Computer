/*
 * Navigation.cpp
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#include "Subsystems/Navigation.h"

#include "main.h"

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus, UART_HandleTypeDef* uart)
	: Subsystem(data),
	  accl(data, spiBus, SPI_CS1_GPIO_Port, SPI_CS1_Pin),
	  baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
	  gps(data, uart)
{

}


int Navigation::init()
{
	if (accl.deviceInit() < 0)
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
	// -------------------------------------------------------------
	// Read Sensor Data
	// -------------------------------------------------------------

	accl.updateDevice();

	baro.updateDevice();

	// -------------------------------------------------------------
	// Quaterion Intergration
	// -------------------------------------------------------------

	// -------------------------------------------------------------
	// Kalman Filter
	// -------------------------------------------------------------

	return 0;
}

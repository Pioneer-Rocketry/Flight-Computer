/*
 * GPS.cpp
 *
 *  Created on: Nov 20, 2025
 *      Author: colin
 */

#include <Devices/GPS.h>


GPS::GPS(DataContainer* data, UART_HandleTypeDef* uart)
{
	this->data = data;

}

int GPS::deviceInit()
{
	return 0;
}

int GPS::updateDevice()
{
	return 0;
}



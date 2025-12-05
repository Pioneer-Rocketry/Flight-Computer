/*
 * Logging.cpp
 *
 *  Created on: Dec 3, 2025
 *      Author: colin
 */

#include "Subsystems/Logging.h"

Logging::Logging(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	  flashChip(data, spiBus, FLASH_CS_GPIO_Port, FLASH_CS_Pin)
{


}

int Logging::init()
{

	return 0;
}

int Logging::update()
{

	return 0;
}

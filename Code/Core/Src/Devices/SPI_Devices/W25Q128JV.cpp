/*
 * W25Q128JV.cpp
 *
 *  Created on: Dec 3, 2025
 *      Author: colin
 */

#include "Devices/SPI_Devices/W25Q128JV.h"

W25Q128JV::W25Q128JV(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
	: SPIDevice(data, spi, port, pin)
{

}

int W25Q128JV::deviceInit()
{

	return 0;
}

int W25Q128JV::updateDevice()
{

	return 0;
}

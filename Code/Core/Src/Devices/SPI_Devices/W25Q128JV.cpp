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
	uint8_t manufacturer[2];

	uint8_t requestID[4] = { 0x00, 0x00, 0x00, 0x00 };

	writeReadSPI(W25Q128VJ_DEVICE_ID, requestID, 4, 0x00, manufacturer, 2);
	if (manufacturer[0] != W25Q128VJ_MANUFACTURER) {
		return -1;
	}

	return 0;
}

int W25Q128JV::updateDevice()
{

	return 0;
}

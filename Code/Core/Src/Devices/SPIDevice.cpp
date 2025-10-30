/*
 * I2CDevice.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/SPIDevice.h"

SPIDevice::SPIDevice(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
{
	this->data = data;

	this->spiHandler = spi;
	this->chipSelectPort = port;
	this->chipSelectPin = pin;
}

HAL_StatusTypeDef SPIDevice::readSPI(uint8_t reg, uint8_t *data, uint8_t len)
{
	uint8_t tx[len+1];
	uint8_t rx[len+1];

	tx[0] = reg;
	for (int i=1; i < len+1; i++) tx[i] = 0x00;
	for (int i=0; i < len+1; i++) rx[i] = 0x00;

	// Pull the SPI Chip Select pin low to select the correct device
	HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

	status = HAL_SPI_TransmitReceive(this->spiHandler, tx, rx, len+1, 10);

	// Pull the SPI Chip Select pin high to deselect the device
	HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

	memcpy(data, rx + 1, len);

	return status;
}


HAL_StatusTypeDef SPIDevice::writeSPI(uint8_t reg, uint8_t *data, uint8_t len)
{
	uint8_t buffer[len + 1];
	buffer[0] = reg;
	memcpy(buffer + 1, data, len);

	// Pull the SPI Chip Select pin low to select the correct device
	HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

	status = HAL_SPI_Transmit(this->spiHandler, buffer, len+1, 10);

	// Pull the SPI Chip Select pin high to deselect the device
	HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

	return status;
}

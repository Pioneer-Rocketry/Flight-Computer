/*
 * I2CDevice.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/I2CDevice.h"

I2CDevice::I2CDevice(DataContainer* data)
{
	this->data = data;
}

HAL_StatusTypeDef I2CDevice::readI2C(uint16_t reg, uint8_t *data, uint16_t len)
{
	return HAL_I2C_Mem_Read(this->i2cHandler, this->address, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef I2CDevice::writeI2C(uint8_t reg, uint8_t *data, uint8_t len)
{
	return HAL_I2C_Mem_Write(this->i2cHandler, this->address, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

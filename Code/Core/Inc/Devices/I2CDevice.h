/*
 * I2CDevice.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEVICES_I2CDEVICE_H_
#define INC_DEVICES_I2CDEVICE_H_

#include <stm32f4xx_hal.h> /* Needed for i2c */

#include "DataContainer.h"

class I2CDevice {
public:
	I2CDevice(DataContainer* data);

	virtual int deviceInit() = 0;

private:
	I2C_HandleTypeDef *i2cHandler;

	uint8_t address;

protected:
	DataContainer* data;

	/*
	 * Read data from the i2c bus
	 *
	 * @param reg The register to read from
	 * @param data The data to be read
	 * @param len The number of bytes to read
	 */
	HAL_StatusTypeDef readI2C(uint16_t reg, uint8_t *data, uint16_t len=1);

	/*
	 * Write data to the i2c bus
	 *
	 * @param reg The register to write to
	 * @param data The data to be written
	 * @param len The number of bytes to write
	 */
	HAL_StatusTypeDef writeI2C(uint8_t reg, uint8_t *data, uint8_t len=1);
};

#endif /* INC_DEVICES_I2CDEVICE_H_ */

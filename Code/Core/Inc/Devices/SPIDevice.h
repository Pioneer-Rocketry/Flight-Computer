/*
 * SPIDevice.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEVICES_SPIDEVICE_H_
#define INC_DEVICES_SPIDEVICE_H_

#include <string.h> /* Needed for memcpy */
#include <stm32f4xx_hal.h> /* Needed for spi */

#include "DataContainer.h"

class SPIDevice {
public:
	SPIDevice(DataContainer* data);

	virtual int deviceInit() = 0;

private:
	HAL_StatusTypeDef status;

protected:
	DataContainer* data;

    SPI_HandleTypeDef *spiHandler;

    GPIO_TypeDef *chipSelectPort;
    uint16_t chipSelectPin;

    /*
	 * Read data from the spi bus
	 *
	 * @param reg The register to read from
	 * @param data The data to be read
	 * @param len The number of bytes to read
	 */
	HAL_StatusTypeDef readSPI(uint8_t reg, uint8_t *data, uint8_t len=1);


	/*
	 * Write data to the spi bus
	 *
	 * @param reg The register to write to
	 * @param data The data to be written
	 * @param len The number of bytes to write
	 */
	HAL_StatusTypeDef write_SPI(uint8_t reg, uint8_t *data, uint8_t len=1);

};

#endif /* INC_DEVICES_SPIDEVICE_H_ */

/*
 * W25Q128JV.h
 *
 *  Created on: Dec 3, 2025
 *      Author: colin
 */

#ifndef INC_DEVICES_SPI_DEVICES_W25Q128JV_H_
#define INC_DEVICES_SPI_DEVICES_W25Q128JV_H_

#include "Devices/SPIDevice.h"

class W25Q128JV: public SPIDevice {
public:
	W25Q128JV(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

	int deviceInit();
	int updateDevice();

private:

};

#endif /* INC_DEVICES_SPI_DEVICES_W25Q128JV_H_ */

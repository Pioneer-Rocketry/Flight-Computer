/*
 * GPS.h
 *
 *  Created on: Nov 20, 2025
 *      Author: colin
 *
 *  Adapted from https://github.com/ibrahimcahit/stm32-ublox-ubx-parser
 */

#ifndef INC_DEVICES_GPS_H_
#define INC_DEVICES_GPS_H_

#include "stm32f4xx_hal.h"

#include "DataContainer.h"

class GPS {
public:
	GPS(DataContainer* data, UART_HandleTypeDef* uart);

	int deviceInit();
	int updateDevice();

private:
	DataContainer* data;

	UART_HandleTypeDef uart;

};

#endif /* INC_DEVICES_GPS_H_ */

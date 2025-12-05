/*
 * Logging.h
 *
 *  Created on: Dec 3, 2025
 *      Author: colin
 */

#ifndef INC_SUBSYSTEMS_LOGGING_H_
#define INC_SUBSYSTEMS_LOGGING_H_

#include "Subsystem.h"
#include "defines.h"
#include "main.h"

#include "Devices/SPI_Devices/W25Q128JV.h"

class Logging: public Subsystem {
public:
	Logging(DataContainer* data, SPI_HandleTypeDef* spiBus);

	int init() override;
	int update() override;

private:
	W25Q128JV flashChip;

	char pageBuffer[NUM_PAGES][PAGE_SIZE];

};

#endif /* INC_SUBSYSTEMS_LOGGING_H_ */

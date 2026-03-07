/*
 * Navigation.h
 *
 *  Created on: Feb 17, 2026
 *      Author: Derek Prince
 */

#ifndef SRC_SUBSYSTEMS_LOGGING_H_
#define SRC_SUBSYSTEMS_LOGGING_H_


#include "Subsystem.h"
#include "main.h"
#include <cstdint>
#include <vector>
//#include "utils.h"
//#include "defines.h"
//#include "Devices/Flash.h"
#include "Devices/SPI_Devices/W25Q128JV.h"
#include "LoggingPacketType.h"

#define LOG_BUFFER_SIZE_BYTES W25Q128JV_PAGE_SIZE - 1 // Using the first byte of a page to keep track of if there is data

class Logging: public Subsystem
{
    public:
	bool enableWriting = true;
    Logging(DataContainer* data, SPI_HandleTypeDef* _spi);

    int init() override;
    int update() override;

	/**
	 * @brief Create a new type of packet (ground, flight, decent, etc)
	 * @param interval in milliseconds to log packet when it is enabled
	 * @returns Pointer to LoggingPacketType to further initilize
	 */
	LoggingPacketType* createPacketType(uint32_t interval_ms);

	/**
	 * @brief Call once after creating packet types and adding all data
	 * sources to initilize the packet buffers
	 * @return 0 on success or error 
	 */
    int createPacketBuffers();

    private:
    W25Q128JV flash;
	uint8_t writeBuffer[LOG_BUFFER_SIZE_BYTES];
	uint16_t logBufferWriteIndex = 0;
	uint16_t currentPage = 0;
	uint16_t currentSector = 0;

    bool loggingEnabled = false;
	std::vector<LoggingPacketType*> packetTypes;

	void writePageToFlash();
};



#endif /* SRC_SUBSYSTEMS_LOGGING_H_ */

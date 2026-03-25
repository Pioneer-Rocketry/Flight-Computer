/*
 * LoggingPacketType.h
 *
 *  Created on: Feb 2, 2026
 *      Author: Derek Prince
 */
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "stm32f4xx_hal.h"

#ifndef INC_SUBSYSTEMS_LOGGING_PACKETTYPE_H_
#define INC_SUBSYSTEMS_LOGGING_PACKETTYPE_H_


class LoggingPacketType{
	public:
	uint8_t packetTypeID;
	bool enabled = true;
	uint8_t* permanentBuffer;
	uint32_t interval_ms = 0;
	uint32_t globalTimeToSendNext_ms = 0;

	LoggingPacketType(uint32_t logInterval_ms){
		interval_ms = logInterval_ms;
	}

	/**
	 * @brief  Get the size of buffer needed to store this packet type
	 * @return packet size in bytes
	 */
	size_t getPacketSize(){
		return packetSize;
	}

	/**
	 * @brief  Collect all data from sources in packetDataSources
	 * @return pointer to buffer of size packetSize
	 */
	uint8_t* collectData(){
		//Structure is [ID][Total Size][Index][Data byte 1][Data byte 2][...]
		//Do we want to add a crc?
		uint32_t millisTimeUInt = HAL_GetTick();
		uint32_t* millisTimePtr = &millisTimeUInt;

		permanentBuffer[0] = packetTypeID;
		permanentBuffer[1] = packetSize;
		permanentBuffer[2] = index++;
		permanentBuffer[3] = ((uint8_t*)millisTimePtr)[0];
		permanentBuffer[4] = ((uint8_t*)millisTimePtr)[1];
		permanentBuffer[5] = ((uint8_t*)millisTimePtr)[2];
		permanentBuffer[6] = ((uint8_t*)millisTimePtr)[3];

		uint32_t address = 7;
		for (uint32_t i = 0; i < packetDataSources.size(); i++){
			permanentBuffer[address] = *(packetDataSources[i]);

			address += packetDataElementSizes[i];
		}

		return permanentBuffer;
	}

	/**
	 * @brief Add a pointer of a source of data to collect for the packet.
	 * @param Pointer of any type to a data element, size is found with demplate
	 * @param Description of the value logged in the first packet, String including null termination should fit in size of data. Excess will be cutoff
	 * @note Expects pointer to never be invalid
	 */
	template <typename T> void addLogDatasource(T* pointer, char[]* description){
		addPacketElement(reinterpret_cast<uint8_t*>(pointer), sizeof(*pointer));
	}

	/**
	 * @brief Set the buffer
	 */
	int setBuffer(uint8_t* buffer, size_t size){
		if (size >= packetSize){
			permanentBuffer = buffer;
			bufferSize = size;
			return 1;
		} else {
			return -1;
		}
	}

private:

	void addPacketElement(uint8_t *pointerToDataSource, size_t size){

		size_t newSize = size;
		for (auto elementSize: packetDataElementSizes){
			newSize += elementSize;
		}
		packetDataSources.push_back(pointerToDataSource);
		packetDataElementSizes.push_back(size);
		packetSize = newSize + 7; // +1 for packet ID and +1 for size
	}

	std::vector<uint8_t*> packetDataSources;
	std::vector<size_t> packetDataElementSizes;
	size_t packetSize;
	size_t bufferSize;
	uint32_t index;
};

#endif
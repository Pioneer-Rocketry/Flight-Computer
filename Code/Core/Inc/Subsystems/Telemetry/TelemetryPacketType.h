/*
 * TelemetryPacketType.h
 *
 *  Created on: Dec 2, 2025
 *      Author: Derek Prince
 */
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "stm32f4xx_hal.h"


#ifndef INC_SUBSYSTEMS_TELEMETRY_PACKETTYPE_H_
#define INC_SUBSYSTEMS_TELEMETRY_PACKETTYPE_H_


class TelemetryPacketType{
public:
	uint8_t packetID;

	uint32_t globalTimeToSendNext_ms = 0;
	uint32_t sendInterval_ms = 0;
	bool enableTypeToSend = true;

	TelemetryPacketType(uint32_t interval_ms){
		sendInterval_ms = interval_ms;
	}

	/* Get the size of buffer needed to store this
	 * packet type
	 */
	size_t getPacketSize(){
		return currentSize;
	}

	uint8_t* collectData(){
		//Structure is [ID][Total Size][Data byte 1][Data byte 2][...]

		uint32_t millisTimeInt = HAL_GetTick();
		uint32_t* millisTimePtr = &millisTimeInt;

		permanentBuffer[0] = packetID;
		permanentBuffer[1] = currentSize;
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

	void addPacketElement(uint8_t *pointerToDataSource, size_t size){

		size_t newSize = size;
		for (auto elementSize: packetDataElementSizes){
			newSize += elementSize;
		}
		packetDataSources.push_back(pointerToDataSource);
		packetDataElementSizes.push_back(size);
		currentSize = newSize + 7; // +1 for packet ID and +1 for size
	}

	int setBuffer(uint8_t* buffer, size_t size){
		if (locked) return -1;

		if (size >= currentSize){
			permanentBuffer = buffer;
			bufferSize = size;
			return 1;
		} else {
			return -1;
		}
	}

private:
	std::vector<uint8_t*> packetDataSources;
	std::vector<size_t> packetDataElementSizes;
	size_t currentSize;
	uint8_t* permanentBuffer;
	size_t bufferSize;
	uint32_t index;
	bool locked; //Lock when create buffers is called to stop from adding more elements
};


#endif /* INC_SUBSYSTEMS_TELEMETRY_PACKETTYPE_H_ */

/*
 * TelemetryPacketType.h
 *
 *  Created on: Dec 2, 2025
 *      Author: Derek Prince
 */
#include <stdint.h>
#include <stdlib.h>
#include <vector>

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

	void collectData(uint8_t *buffer, size_t bufferSize){
		//Structure is [ID][Total Size][Data byte 1][Data byte 2][...]

		if (bufferSize < currentSize){
			return;
		}

		buffer[0] = packetID;
		buffer[1] = currentSize;

		uint32_t address = 2;
		for (uint32_t i = 0; i < packetDataSources.size(); i++){
			buffer[address] = *(packetDataSources[i]);

			address += packetDataElementSizes[i];
		}
	}

	void addPacketElement(uint8_t *pointerToDataSource, size_t size){

		size_t newSize = size;
		for (auto elementSize: packetDataElementSizes){
			newSize += elementSize;
		}
		packetDataSources.push_back(pointerToDataSource);
		packetDataElementSizes.push_back(size);
		currentSize = newSize + 2; // +1 for packet ID and +1 for size
	}

private:
	std::vector<uint8_t*> packetDataSources;
	std::vector<size_t> packetDataElementSizes;
	size_t currentSize;
};


#endif /* INC_SUBSYSTEMS_TELEMETRY_PACKETTYPE_H_ */

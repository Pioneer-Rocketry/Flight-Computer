/*
 * Navigation.cpp
 *
 *  Created on: Feb 18, 2026
 *      Author: Derek Prince
 */

#include "Subsystems/Logging.h"
#include "usbHelper.h"


Logging::Logging(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	flash(spiBus, FLASH_CS_GPIO_Port, FLASH_CS_Pin)
{

}

void Logging::writePageToFlash(){
	if (currentPage << 4 != currentSector){ //Moved into next sector
		flash.eraseSector(currentPage << 4);
		currentSector = currentPage << 4;
	}

	flash.writePage(currentPage << 8, writeBuffer, 255);
	currentPage++;
	if (currentPage > 2^16){
		enableWriting = false;
	}
}

int Logging::init()
{
	memset(writeBuffer, 0xFF, 256);
	logBufferWriteIndex = 1; // Use First byte as indicator to know if page has been written to
	writeBuffer[0] = 0;

	//Read first byte of each page to see if it has data, set current page to next
	uint32_t i = 0;
	uint8_t readByte = 0xFF;
	do {
		readByte = flash.readByte(i);
		uint8_t size = flash.readByte(i+1);
		i += size;

		if (i > 65535){
			return -1; // Flash is full
		}

	} while (readByte != 0xFF); // Empty flash is filled with 0xFF

	currentPage = i; //Choose first unfilled page
	//First byte of each packet is the packet ID, which start at 1 and go up, so if 
	currentSector = currentPage >> 4; //16 pages per sector


	return 0;
}

int Logging::update()
{
	uint32_t millis = HAL_GetTick();
	for (LoggingPacketType* packet: packetTypes){
		if (millis >= packet->globalTimeToSendNext_ms){
			packet->globalTimeToSendNext_ms += packet->interval_ms;
			packet->collectData();
			
			uint16_t packetReadIndex = 0;
			uint16_t packetSize = packet->getPacketSize();

			//Fill buffer until full then dump to flash
			//Should work with packets sizes smaller and larger than page sizes
			while(packetReadIndex < packetSize){
				uint8_t writeSize = std::min(packetSize - packetReadIndex, LOG_BUFFER_SIZE_BYTES);

				memcpy(&(packet->permanentBuffer[packetReadIndex]), &(writeBuffer[logBufferWriteIndex]), writeSize);
				logBufferWriteIndex += writeSize;
				packetReadIndex += writeSize;

				if (logBufferWriteIndex >= LOG_BUFFER_SIZE_BYTES){
					if (enableWriting){
						Logging::writePageToFlash();
					}
					memset(writeBuffer, 0xFF, LOG_BUFFER_SIZE_BYTES); // Clear buffer

					writeBuffer[0] = 0;
					logBufferWriteIndex = 1; // Use first byte as indicator to know if a page has been written to
				}
			}

		}
	}
	return 0;
}

//Create a new type of packet
//Returns the new packet type object to allow for its initialization of adding data sources
LoggingPacketType* Logging::createPacketType(uint32_t interval_ms){
	LoggingPacketType* newPacketType = new LoggingPacketType(interval_ms);
	newPacketType->packetTypeID = packetTypes.size() + 1;
	packetTypes.push_back(newPacketType);
	return newPacketType;
}

int Logging::createPacketBuffers(){
	for (LoggingPacketType* packetType: packetTypes){
		uint8_t* newBuffer = (uint8_t*)malloc(packetType->getPacketSize()); //Delete is called by the electrons leaking out of the sram cells
		if (newBuffer == nullptr){
			return -1;
		}
		packetType->setBuffer(newBuffer, packetType->getPacketSize());
	}
	return 0;
}

/*void Logging::SetMode(int newMode){

}*/
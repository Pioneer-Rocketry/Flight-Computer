/*
 * Navigation.cpp
 *
 *  Created on: Feb 18, 2026
 *      Author: Derek Prince
 */

#include "Subsystems/Logging.h"
#include "Devices/SPI_Devices/W25Q128JV.h"
#include "usbHelper.h"

#include <cstdio>
#include <cstring>

Logging::Logging(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	flash(data, spiBus, FLASH_CS_GPIO_Port, FLASH_CS_Pin)
{

}

int Logging::init()
{
	flash.init();
	//Read first byte of each page to see if it has data, set current page to next
	uint32_t i = 0;
	uint8_t readByte = 0xFF;
	do {
		readByte = flash.readByte(i);
		i += W25Q128JV_PAGE_SIZE;

		if (i > 65535){
			// return -1; // Flash is full
		}


	} while (readByte != 0xFF && i < W25Q128JV_PAGE_SIZE*W25Q128JV_PAGE_COUNT); // Empty flash is filled with 0xFF

	// currentPage = i; //Choose first unfilled page
	// //First byte of each packet is the packet ID, which start at 1 and go up, so if
	// currentSector = currentPage >> 4; //16 pages per sector

	currentPage = 0;
	currentSector = -1;

	return 0;
}

int Logging::update()
{
	// if (currentPage >> 12 != currentSector){ //Moved into next sector
	// 	currentSector = currentPage >> 12;
	// 	flash.eraseSector(currentSector);
	// }

	if (enableWriting){
		#define x(type, name) flashPacket.fields.name = data->name;
		LOGGING_PACKET_FIELDS(x)
		#undef x

		flash.writePage(currentPage, flashPacket.raw, sizeof(flashPacket.raw));
		currentPage++;
	}

	if (currentPage > 0x10000){
		enableWriting = false;
	}

	return 0;
}

void Logging::dumpFlash(){
	uint8_t pageData[256];
	char msg[1024];
	uint16_t msgLen;

	// Print CSV header
	msgLen = 0;
	
	#define LOGGING_HEADER_float(name) \
		if (msgLen < (int)sizeof(msg)) { \
			msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, ",%s", #name); \
		}

	#define LOGGING_HEADER_int(name) \
		if (msgLen < (int)sizeof(msg)) { \
			msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, ",%s", #name); \
		}

	#define LOGGING_HEADER(type, name) LOGGING_HEADER_##type(name)
	LOGGING_PACKET_FIELDS(LOGGING_HEADER)
	#undef LOGGING_HEADER
	#undef LOGGING_HEADER_float
	#undef LOGGING_HEADER_int

	msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, "\r\n");
	cdcSendMessage(msg, msgLen);
	tud_task();

	// Print data rows
	for (uint32_t pageNum = 0; pageNum < W25Q128JV_PAGE_COUNT; pageNum++)
	{
		flash.readData(pageNum << 8, pageData, sizeof(flashPacket.raw));

		memcpy(flashPacket.raw, pageData, sizeof(flashPacket.raw));

		msgLen = 0;

		#define LOGGING_DATA_float(name) \
			if (msgLen < (int)sizeof(msg)) { \
				msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, ",%.6f", flashPacket.fields.name); \
			}

		#define LOGGING_DATA_int(name) \
			if (msgLen < (int)sizeof(msg)) { \
				msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, ",%d", flashPacket.fields.name); \
			}

		#define LOGGING_DATA(type, name) LOGGING_DATA_##type(name)
		LOGGING_PACKET_FIELDS(LOGGING_DATA)
		#undef LOGGING_DATA
		#undef LOGGING_DATA_float
		#undef LOGGING_DATA_int

		if (msgLen < 0) {
			msgLen = 0;
		} else if ((size_t)msgLen >= sizeof(msg)) {
			msgLen = sizeof(msg) - 1;
		}

		msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, "\r\n");
		cdcSendMessage(msg, msgLen);
		tud_task();
	}
}

void Logging::erase(){
	for (uint32_t sector = 0; sector < W25Q128JV_NUMBER_OF_SECTORS; sector++){
		flash.eraseSector(sector);
	}

	currentPage = 0;
}
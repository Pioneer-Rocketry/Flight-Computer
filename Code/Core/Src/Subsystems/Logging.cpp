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

int Logging::init()
{
	flash.init();
	//Read first byte of each page to see if it has data, set current page to next
	uint32_t i = 0;
	uint8_t readByte = 0xFF;
	do {
		readByte = flash.readByte(i += W25Q128JV_PAGE_SIZE);

		if (i > 65535){
			return -1; // Flash is full
		}


	} while (readByte != 0xFF && i < W25Q128JV_PAGE_SIZE*W25Q128JV_PAGE_COUNT); // Empty flash is filled with 0xFF

	currentPage = i; //Choose first unfilled page
	//First byte of each packet is the packet ID, which start at 1 and go up, so if
	currentSector = currentPage >> 4; //16 pages per sector

	return 0;
}

int Logging::update()
{
	// if (currentPage << 4 != currentSector){ //Moved into next sector
	// 	flash.eraseSector(currentPage << 4);
	// 	currentSector = currentPage << 4;
	// }

	if (enableWriting){
		#define x(type, name) flashPacket.fields.name = data->name;
		LOGGING_PACKET_FIELDS(x)
		#undef x

		flash.writePage(currentPage, flashPacket.raw, sizeof(flashPacket.raw));
	}

	currentPage++;
	if (currentPage > 2^16){
		enableWriting = false;
	}

	return 0;
}

void Logging::dumpFlash(){
	uint8_t pageData[256];
	char msg[350];
	int msgLen = 0;

	for (uint16_t pageNum = 0; pageNum < W25Q128JV_PAGE_COUNT; pageNum++)
	{
		flash.readData(pageNum, pageData, W25Q128JV_PAGE_SIZE);

		msgLen = snprintf(msg, sizeof(msg), "0x%04X: ", pageNum);
		for (uint16_t i = 0; i < W25Q128JV_PAGE_SIZE; i++)
			msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, "%02X ", pageData[i]);
		msgLen += snprintf(msg + msgLen, sizeof(msg) - msgLen, "\r\n\r\n\r\n\r\n");

		cdcSendMessage((char*)msg, msgLen);
		tud_task();

		HAL_Delay(1000);
	}
}

void Logging::erase(){
	for (uint16_t sector = 0; sector < W25128JV_SECTOR_SIZE; sector++){
		flash.eraseSector(sector);
	}
}
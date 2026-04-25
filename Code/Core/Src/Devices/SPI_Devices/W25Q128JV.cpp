/*
 * MS560702BA03.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/SPI_Devices/W25Q128JV.h"

W25Q128JV::W25Q128JV(SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
	: SPIDevice(data, spi, port, pin)
{
}

int W25Q128JV::init()
{
	if (writeSPI(W25Q128JV_ENABLE_RESET, nullptr, 0) != HAL_OK) {
		return -1;
	}

    if (writeSPI(W25Q128JV_RESET_DEVICE, nullptr, 0) != HAL_OK) {
		return -1;
	}

	delay_us(60); // Approximate reset time * 2

	if (readManufacturer() != 0xef){
		//error, incorrect device
		return -1;
	}

	if (readDeviceID() != 0x17){
		//error, incorrect device
		return -1;
	}

	writeSPI(W25Q128JV_WRITE_ENABLE, nullptr, 0); // Enable Write

	return 0;
}

uint64_t W25Q128JV::readUUID(){
    uint8_t data[12];
	uint64_t ID;
    readSPI(W25Q128JV_READ_UNIQUE_ID, data, 12);
	for (uint8_t i = 0; i < 8; i++){
		ID = ID << 8;
		ID |= data[4+i];
	}
    return ID;
}

uint8_t W25Q128JV::readManufacturer(){
    uint8_t data[5];
    readSPI(W25Q128JV_MANUFACTURER_DEVICE_ID, data, 5);
    return data[3];
}

uint8_t W25Q128JV::readDeviceID(){
    uint8_t data[5];
    readSPI(W25Q128JV_MANUFACTURER_DEVICE_ID, data, 5);
    return data[4];
}

int W25Q128JV::update(){
	return 0;
}

uint8_t W25Q128JV::readByte(uint32_t address){
	uint8_t data;
	readSPI(W25128JV_READ_DATA, &data, 1);
	return data;
}

HAL_StatusTypeDef W25Q128JV::readData(uint32_t address, uint8_t* buffer, uint32_t length){
	uint8_t pageAddress[3];
	pageAddress[0] = (address & (0x00FF0000)) >> 16;
	pageAddress[1] = (address & (0x0000FF00)) >> 8;
	pageAddress[2] = (address & (0x000000FF));

	writeSPI(W25128JV_READ_DATA, pageAddress, 3);
	return readSPI(W25128JV_READ_DATA, buffer, length);
}

void W25Q128JV::eraseSector(uint16_t sector){
	writeSPI(W25Q128JV_WRITE_ENABLE, nullptr, 0); // Enable Write

	uint8_t sectorAddress[3];
	sectorAddress[0] = (sector & (0x00FF0000)) >> 16;
	sectorAddress[1] = (sector & (0x0000FF00)) >> 8;
	sectorAddress[2] = (sector & (0x000000FF));

	writeSPI(W25128JV_SECTOR_ERASE_4KB, sectorAddress, 3);

	//Is this necessary? Or should it be moved to the write page function?
	//Wait for busy flag to clear
	while ((readSPI(W25128JV_READ_STATUS_REG_1, nullptr, 0) & W25128JV_REG_1_WRITE_IN_PROGRESS) != 0){
		;;
	}

}

void W25Q128JV::writePage(uint32_t page, uint8_t* pageData, uint8_t size){
	//uint8_t spiBuffer[4]; //24 bit address

	//spiBuffer[0] = (page & (0x00FF0000)) >> 16;
	//spiBuffer[1] = (page & (0x0000FF00)) >> 8;
	//spiBuffer[2] = (page & (0x000000FF));

	// This could be faster with a memcopy
	// memcpy(&spiBuffer, page, 4);

	if (size > W25Q128JV_PAGE_SIZE){
		size = W25Q128JV_PAGE_SIZE;
	}

	selectDevice();

	uint8_t pageAddress[3];
	pageAddress[0] = (page & (0x00FF0000)) >> 16;
	pageAddress[1] = (page & (0x0000FF00)) >> 8;
	pageAddress[2] = (page & (0x000000FF));

	writeSPIRaw(W25128JV_PAGE_PROGRAM);
	writeSPIRaw(pageAddress, 3);

	uint8_t packetUsedByte = 0x01;
	writeSPIRaw(&packetUsedByte, 1); //Set first byte of page to be written
	writeSPIRaw(pageData, size);

	deselectDevice();
}
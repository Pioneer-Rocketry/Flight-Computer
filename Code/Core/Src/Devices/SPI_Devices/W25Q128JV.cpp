/*
 * MS560702BA03.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/SPI_Devices/W25Q128JV.h"

W25Q128JV::W25Q128JV(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
	: SPIDevice(data, spi, port, pin)
{
}

int W25Q128JV::deviceInit()
{
	if (writeSPI(W25Q128JV_ENABLE_RESET, nullptr, 0) != HAL_OK) {
		return -1;
	}

    if (writeSPI(W25Q128JV_RESET_DEVICE, nullptr, 0) != HAL_OK) {
		return -1;
	}

	delay_us(60); // Approximate reset time * 2

	/* Flash Configuration */
	uint8_t config[3];

	// Statys Register1 page 58, table 32
	config[0]  = 0;
	config[0] |= 0b0 	<< 7; // Status Register Protect (Read Only)
	config[0] |= 0b0 	<< 6; // Sector Protect (Read Only)
	config[0] |= 0b000 	<< 3; // Block Protect Bits
	config[0] |= 0b0 	<< 2; // Top/Bottom Protect
	config[0] |= 0b0 	<< 1; // Write Enable Latch (Read Only)
	config[0] |= 0b0 	<< 0; // Write In Progress (Read Only)

	config[1]  = 0;
	config[1] |= 0b0 	<< 7; // Suspend Status (Read Only)
	config[1] |= 0b0 	<< 6; // Component Protect
	config[1] |= 0b000 	<< 3; // Security Register Lock Bits
	config[1] |= 0b0 	<< 2; // Reserved
	config[1] |= 0b0 	<< 1; // Quad Enable (QE) Bit
	config[1] |= 0b0 	<< 0; // Status Register Lock

	config[2]  = 0;
	config[2] |= 0b0 	<< 7; // /HOLD - /RESET
	config[2] |= 0b00 	<< 5; // Output Driver Strength
	config[2] |= 0b0 	<< 4; // Reserved
	config[2] |= 0b0 	<< 3; // Reserved
	config[2] |= 0b0 	<< 2; // Write Protect Selection
	config[2] |= 0b0 	<< 1; // Reserved
	config[2] |= 0b0 	<< 0; // Reserved
	writeSPI(W25Q128JV_READ_STATUS_REG_1, config, 3);

	HAL_Delay(10); // Wait for write to complete

	if (readManufacturer() != 0xef){
		//error, incorrect device
		return -1;
	}

	if (readDeviceID() != 0x17){
		//error, incorrect device
		return -1;
	}

	return 0;
}

uint64_t W25Q128JV::readUUID(){
    uint8_t data[12];
	uint64_t ID = 0;
    if (readSPI(W25Q128JV_READ_UNIQUE_ID, data, 12) != HAL_OK) {
        return 0;
    }
	for (uint8_t i = 0; i < 8; i++){
		ID = (ID << 8) | data[4 + i];
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

int W25Q128JV::updateDevice(){
	return 0;
}

uint8_t W25Q128JV::readByte(uint32_t address){
	uint8_t data;
	readData(address, &data, 1);
	return data;
}

HAL_StatusTypeDef W25Q128JV::readData(uint32_t address, uint8_t* buffer, uint32_t length){
	uint8_t pageAddress[3];
	pageAddress[0] = (address & (0x00FF0000)) >> 16;
	pageAddress[1] = (address & (0x0000FF00)) >> 8;
	pageAddress[2] = (address & (0x000000FF));

	selectDevice();

	writeSPIRaw(W25Q128JV_READ_DATA);
	writeSPIRaw(pageAddress, 3);

	return readSPI(0x00, buffer, length);
}

void W25Q128JV::eraseSector(uint16_t sector){
	writeSPI(W25Q128JV_WRITE_ENABLE, nullptr, 0); // Enable Write

	sector <<= 12; // Convert sector number to byte address

	uint8_t sectorAddress[3];
	sectorAddress[0] = (sector & (0x00FF0000)) >> 16;
	sectorAddress[1] = (sector & (0x0000FF00)) >> 8;
	sectorAddress[2] = (sector & (0x000000FF));

	writeSPI(W25Q128JV_SECTOR_ERASE_4KB, sectorAddress, 3);

	waitForWrite();
}

void W25Q128JV::writePage(uint32_t page, uint8_t* pageData, uint8_t size){
	if (size > (uint8_t)W25Q128JV_PAGE_SIZE){
		size = (uint8_t)W25Q128JV_PAGE_SIZE;
	}

	writeSPI(W25Q128JV_WRITE_ENABLE, nullptr, 0); // Enable Write

	selectDevice();

	uint8_t pageAddress[3];
	pageAddress[0] = (page & (0x0000FF00)) >> 8;
	pageAddress[1] = (page & (0x000000FF));
	pageAddress[2] = 0x00;

	writeSPIRaw(W25Q128JV_PAGE_PROGRAM);
	writeSPIRaw(pageAddress, 3);

	uint8_t packetUsedByte = 0x01;
	writeSPIRaw(&packetUsedByte, 1); //Set first byte of page to be written
	writeSPIRaw(pageData, size);

	deselectDevice();

	waitForWrite();
}

void W25Q128JV::waitForWrite(){
	
	uint8_t status;
	//Is this necessary? Or should it be moved to the write page function?
	//Wait for busy flag to clear
	do {
		readSPI(W25Q128JV_READ_STATUS_REG_1, &status, 1);
	}
	while ((status & 0x01) != 0);
}
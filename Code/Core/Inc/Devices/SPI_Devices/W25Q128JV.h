/*
 * MS560702BA03.h
 *
 *  Created on: Feb 19, 2026
 *      Author: Derek Prince
 */

#ifndef SRC_DEVICES_SPI_DEVICES_W25Q128JV_H_
#define SRC_DEVICES_SPI_DEVICES_W25Q128JV_H_

#include "Devices/Flash.h"
#include "Devices/SPIDevice.h"
#include "defines.h"
#include "utils.h"
#include "main.h"
#include <cstdint>

/*All taken from the datasheet "W25Q128JV 
Publication Release Date: August 23, 2021 
Revision I
3V 128M-BIT
SERIAL FLASH MEMORY WITH
DUAL/QUAD SPI
*/

// Instructions
#define W25Q128JV_WRITE_ENABLE 0x06
#define W25128JV_SR_WRITE_ENABLE 0x50
#define W25Q128JV_WRITE_DISABLE 0x04

#define W25128JV_RELEASE_POWERDOWN_ID 0xAB
#define W25Q128JV_MANUFACTURER_DEVICE_ID 0x90
#define W25Q128JV_JEDEC_ID 0x9F
#define W25Q128JV_READ_UNIQUE_ID 0x4B

#define W25128JV_READ_DATA 0x03
#define W25128JV_FAST_READ 0x0B

#define W25128JV_PAGE_PROGRAM 0x02

#define W25128JV_SECTOR_ERASE_4KB 0x20
#define W25128JV_BLOCK_ERASE_32KB 0x52
#define W25128JV_BLOCK_ERASE_64KB 0xD8
#define W25128JV_CHIP_ERASE 1 0xC7
#define W25128JV_CHIP_ERASE_2 0x60

#define W25128JV_READ_STATUS_REG_1 0x05
#define W25128JV_WRTIE_STATUS_REG_1 0x01
#define W25128JV_READ_STATUS_REG_2 0x35
#define W25128JV_WRITE_STATUS_REG_2 0x31
#define W25128JV_READ_STATUS_REG_3 0x15
#define W25128JV_WRITE_STATUS_REG_3 0x11

#define W25128JV_READ_SFDP_REG 0x5A
#define W25128JV_ERASE_SECURITY_REG 0x44
#define W25128JV_PROGRAM_SECURITY_REG 0x42
#define W25128JV_READ_SECURITY_REG 0x48

#define W25128JV_GLOBAL_BLOCK_LOCK 0x7E
#define W25128JV_GLOBAL_BLOCK_UNLOCK 0x98
#define W25128JV_READ_BLOCK_LOCK 0x3D
#define W25128JV_INDIVIDUAL_BLOCK_LOCK 0x36
#define W25128JV_INDIVIDUAL_BLOCK_UNLOCK 0x39

#define W25128JV_ERASE_PROGRAM_SUSPEND 0x75
#define W25128JV_ERASE_PROGRAM_RESUME 0x7A

#define W25Q128JV_ENABLE_RESET 0x66
#define W25Q128JV_RESET_DEVICE 0x99

//Register bits
#define W25128JV_REG_1_STATUS_REGISTER_PROTECT 0x80
#define W25128JV_REG_1_SECTOR_PROTECT 0x40
#define W25128JV_REG_1_TOP_BOTTOM_PROTECT 0x20
#define W25128JV_REG_1_BLOCK_PROTECT_BITS 0x1C
#define W25128JV_REG_1_WRITE_ENABLE_LATCH 0x02
#define W25128JV_REG_1_WRITE_IN_PROGRESS 0x01

#define W25128JV_REG_2_SUSPEND_STATUS 0x80
#define W25128JV_REG_2_COMPLEMENT_PROTECT 0x40
#define W25128JV_REG_2_SECURITY_REGISTER_LOCK_BITS 0x38
#define W25128JV_REG_2_RESERVED 0x04
#define W25128JV_REG_2_QUAD_ENABLE 0x02
#define W25128JV_REG_2_STATUS_REGISTER_LOCK 0x01

#define W25128JV_REG_3_N_HOLD_OR_N_RESET 0x80
#define W25128JV_REG_3_OUTPUT_DRIVER_STRENGTH 0x40
#define W25128JV_REG_3_RESERVED_1 0x18
#define W25128JV_REG_3_WRITE_PROTECT_SELECTION 0x04
#define W25128JV_REG_3_RESERVED_2 0x03

#define W25Q128JV_PAGE_SIZE 256
#define W25128JV_SECTOR_SIZE 4096

//Registers

/**
 * @class W25Q128JV
 * @brief Driver for the W25Q128JV Flash.
 *
 * This class implements an SPI-based driver for the W25Q128JV Flash.
 *
 * The driver communicates via SPI using the `SPIDevice` base class and
 * implements a device-specific initialization routine (`deviceInit()`).
 *
 * Configuration options are based on the official winbond datasheet.
 * Default values are selected for typical operation.
 *
 * @see winbond W25Q128JV Datasheet, Rev 1, August 2021
 */
class W25Q128JV: public SPIDevice {
public:

	/**
	 * @brief Construct a new W25Q128JV device.
	 *
	 * Initializes the W25Q128JV SPI driver interface and sets default
	 * configuration parameters for the Flash. The SPI interface, CS port,
	 * and pin are passed to the base `SPIDevice` class.
	 *
	 * @param data Pointer to the main DataContainer used for shared system data.
	 * @param spi Pointer to the SPI handle (SPI_HandleTypeDef).
	 * @param port Pointer to the GPIO port controlling the chip select (CS) pin.
	 * @param pin GPIO pin number used for chip select (CS).
	 */
	W25Q128JV(SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

	/**
	 * @brief Initialize the W25Q128JV device.
	 *
	 * Configures the Flash by writing default settings to key control registers,
	 * verifying communication with the device, and preparing it for operation.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	int deviceInit() override;

	//Not used for this device
	int updateDevice() override;

    /**
	 * @brief Read the UUID of the W25Q128JV device.
	 * 
	 * @return uint64_t unique chip id
	 */
	uint64_t readUUID();

	/**
	 * @brief Read the mmanufacturer of the W25Q128JV device. Should be 0xef
	 * 
	 * @return uint8_t Manufacturer ID
	 */
	uint8_t readManufacturer();

	/**
	 * @brief Read the device ID of the W25Q128JV device. Should be 0x17
	 * 
	 * @return uint8_t Device ID
	 */
	uint8_t readDeviceID();

	/**
	 * @brief Reads asingle  byte of data from an address of the W25Q128JV device.
	 *
	 * @param address Address of the byte
	 * @return uint8_t byte
	 */
	uint8_t readByte(uint32_t address);

	/**
	 * @brief Read a byte of data from an address of the W25Q128JV device.
	 *
	 * @param address Byte address of the start of the data
	 * @param buffer Pointer to buffer large enough to contain requested data
	 * @param length Total length of data to read, can read entire chip in one command
	 * @return HAL_StatusTypeDef HAL_OK if successful, or an appropriate HAL error code.
	 */
	HAL_StatusTypeDef readData(uint32_t address, uint8_t* buffer, uint32_t length);

	/**
	 * @brief Program up to a page (256 bytes) of data on the W25Q128JV device. 
	 * @note Page must be erased with eraseSector befre a write can be completed.
	 * Writes wrap around to the start of the page if they extend past the end of
	 * a page, so to program a full page the address needs to be page aligned.
	 *
	 * @param address Byte address of the start of the data
	 * @param buffer Pointer to buffer of data to be programmed
	 * @param length Total length of data to programmed, between 1 and 256
	 * @return void
	 */
	void writePage(uint32_t page, uint8_t* array, uint8_t size);

	/**
	 * @brief Erases a sector (4096 bytes) of flash to allow it to be written to
	 *
	 * @param address Byte address of the sector to be erased
	 * @return void
	 */
	void eraseSector(uint16_t sector);
	
};

#endif /* SRC_DEVICES_SPI_DEVICES_MS560702BA03_H_ */

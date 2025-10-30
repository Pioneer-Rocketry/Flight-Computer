/*
 * SPIDevice.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEVICES_SPIDEVICE_H_
#define INC_DEVICES_SPIDEVICE_H_

#include <string.h> /* Needed for memcpy */
#include <stm32f4xx_hal.h> /* Needed for spi */

#include "DataContainer.h"

/**
 * @class SPIDevice
 * @brief Abstract base class for SPI-based peripheral devices.
 *
 * The SPIDevice class provides a generic interface for communicating
 * with devices over the SPI bus on an STM32 microcontroller. It handles
 * low-level SPI transactions such as reading from and writing to device
 * registers while allowing derived classes to implement device-specific
 * initialization and functionality.
 *
 * This class is designed to be inherited by specific SPI device drivers
 * (e.g., sensors, memory chips, ADCs, etc.) which will implement their
 * own initialization and data handling routines.
 *
 * @note This class assumes SPI is configured and initialized externally
 *       before calling any read or write operations.
 */
class SPIDevice {
public:

	/**
	 * @brief Construct a new SPIDevice object.
	 *
	 * Initializes a generic SPI device interface and stores references to
	 * the SPI peripheral, chip select (CS) pin, and main DataContainer.
	 * This constructor does not perform any hardware initialization; it
	 * simply sets up references for derived device classes.
	 *
	 * @param data Pointer to the main DataContainer used for sharing system data.
	 * @param spi Pointer to the SPI handle (SPI_HandleTypeDef) used for communication.
	 * @param port Pointer to the GPIO port that controls the chip select (CS) pin.
	 * @param pin GPIO pin number used for the chip select (CS) signal.
	 */
	SPIDevice(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

	/**
	 * @brief Initialize the SPI device.
	 *
	 * Must be implemented by derived classes to configure device registers,
	 * verify communication, or perform startup routines.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	virtual int deviceInit() = 0;

	/**
	 * @brief Perform periodic device tasks or updates.
	 *
	 * This pure virtual method should be implemented by derived SPI device classes
	 * to execute regular operations that need to run in the main loop, such as:
	 *  - Reading sensor data
	 *  - Updating internal state
	 *  - Performing background processing or checks
	 *
	 * The method is intended to be called periodically, typically from the main
	 * system loop or a scheduler.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	virtual int updateDevice() = 0;

private:
	HAL_StatusTypeDef status;		// SPI read/write status

protected:
	DataContainer* data;			// Pointer to the shared DataContainer.

    SPI_HandleTypeDef *spiHandler;	// SPI peripheral handle.
    GPIO_TypeDef *chipSelectPort;	// Pointer to the port connected to the Chip Select Pin
    uint16_t chipSelectPin;			// The Chip Select Pin

    /**
     * @brief Read data from an SPI device register.
     *
     * Sends a register address to the SPI device and reads back one or more bytes
     * of data. The chip select (CS) line is automatically asserted low before the
     * transaction and released high afterward.
     *
     * @param reg Register address to read from.
     * @param data Pointer to the buffer where the received data will be stored.
     * @param len Number of bytes to read (default = 1).
     * @return HAL_StatusTypeDef HAL_OK if successful, or an appropriate HAL error code.
     *
     * @note This function assumes the SPI peripheral has been properly initialized
     *       before calling. It should typically be used by higher-level device
     *       functions to read configuration or sensor data.
     */
	HAL_StatusTypeDef readSPI(uint8_t reg, uint8_t *data, uint8_t len=1);


	/**
	 * @brief Write data to an SPI device register.
	 *
	 * Sends a register address followed by one or more bytes of data to the SPI
	 * device. The chip select (CS) line is automatically asserted low before the
	 * transaction and released high afterward.
	 *
	 * @param reg Register address to write to.
	 * @param data Pointer to the buffer containing the data to send.
	 * @param len Number of bytes to write (default = 1).
	 * @return HAL_StatusTypeDef HAL_OK if successful, or an appropriate HAL error code.
	 *
	 * @note This function assumes the SPI peripheral has been properly initialized
	 *       before calling. It should typically be used by higher-level device
	 *       functions to configure registers or send commands.
	 */
	HAL_StatusTypeDef writeSPI(uint8_t reg, uint8_t *data, uint8_t len=1);

};

#endif /* INC_DEVICES_SPIDEVICE_H_ */

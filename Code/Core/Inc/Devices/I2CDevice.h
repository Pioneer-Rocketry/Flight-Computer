/*
 * I2CDevice.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEVICES_I2CDEVICE_H_
#define INC_DEVICES_I2CDEVICE_H_

#include <stm32f4xx_hal.h> /* Needed for i2c */

#include "DataContainer.h"

/**
 * @class I2CDevice
 * @brief Abstract base class for I2C-based peripheral devices.
 *
 * The I2CDevice class provides a generic interface for communicating
 * with I2C peripherals on an STM32 microcontroller. It manages
 * low-level I2C read and write transactions while allowing derived
 * classes to implement device-specific initialization and logic.
 *
 * Derived classes should implement `deviceInit()` to configure the
 * specific hardware device (e.g., sensors, EEPROMs, ADCs).
 * *
 * @note The I2C peripheral must be initialized before using this class.
 */
class I2CDevice {
public:
	/**
	 * @brief Construct a new I2CDevice object.
	 *
	 * Initializes a generic I2C device interface and stores a reference
	 * to the shared DataContainer for system-wide access.
	 * The specific I2C peripheral and device address should be
	 * configured by the derived class or initialization routine.
	 *
	 * @param data Pointer to the main DataContainer used for sharing system data.
	 */
	I2CDevice(DataContainer* data);

	/**
	 * @brief Initialize the I2C device.
	 *
	 * Must be implemented by derived classes to configure device registers,
	 * verify communication, or perform startup routines.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	virtual int deviceInit() = 0;

private:
	I2C_HandleTypeDef *i2cHandler;	// I2C peripheral handle.
	uint8_t address;				// 7-bit or 8-bit I2C device address.

protected:
	DataContainer* data; // Pointer to the shared DataContainer.

	/**
	 * @brief Read data from an I2C device register.
	 *
	 * Reads one or more bytes from a specific register of the I2C device.
	 *
	 * @param reg Register address to read from.
	 * @param data Pointer to the buffer to store the received data.
	 * @param len Number of bytes to read (default = 1).
	 * @return HAL_StatusTypeDef HAL_OK if successful, or an appropriate HAL error code.
	 *
	 * @note This function assumes the I2C peripheral has been properly
	 *       initialized before calling. It should typically be used
	 *       by higher-level device-specific read functions.
	 */
	HAL_StatusTypeDef readI2C(uint16_t reg, uint8_t *data, uint16_t len=1);

	/**
	 * @brief Write data to an I2C device register.
	 *
	 * Writes one or more bytes to a specific register of the I2C device.
	 *
	 * @param reg Register address to write to.
	 * @param data Pointer to the buffer containing the data to write.
	 * @param len Number of bytes to write (default = 1).
	 * @return HAL_StatusTypeDef HAL_OK if successful, or an appropriate HAL error code.
	 *
	 * @note This function assumes the I2C peripheral has been properly
	 *       initialized before calling. It should typically be used
	 *       by higher-level device-specific write functions.
	 */
	HAL_StatusTypeDef writeI2C(uint8_t reg, uint8_t *data, uint8_t len=1);
};

#endif /* INC_DEVICES_I2CDEVICE_H_ */

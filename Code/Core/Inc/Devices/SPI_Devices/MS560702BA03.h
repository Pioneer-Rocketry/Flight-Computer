/*
 * MS560702BA03.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef SRC_DEVICES_SPI_DEVICES_MS560702BA03_H_
#define SRC_DEVICES_SPI_DEVICES_MS560702BA03_H_

#include "Devices/SPIDevice.h"

#include "defines.h"
// Address
#define MS5607_ADDRESS 0b1110111 << 1 // or 0b1110110 if CSB is low

// Commands
#define MS5607_RESET        0x1E
#define MS5607_CONVERT_D1   0x40 // Pressure conversion
#define MS5607_CONVERT_D2   0x50 // Temperature conversion
#define MS5607_ADC_READ     0x00
#define MS5607_PROM_READ    0xA0 // Base address for PROM read


/**
 * @class MS560702BA03
 * @brief Driver for the MS560702BA03 Barometer.
 *
 * This class implements an SPI-based driver for the MS560702BA03 Barometer,
 * It provides enumerations for the configuration options such output data rate (ODR), and operating mode.
 *
 * The driver communicates via SPI using the `SPIDevice` base class and
 * implements a device-specific initialization routine (`deviceInit()`).
 *
 * Configuration options are based on the official STMicroelectronics datasheet.
 * Default values are selected for typical performance-oriented operation.
 *
 * @see TE MS560702BA03 Datasheet, Rev B3, March 2024
 */
class MS560702BA03: public SPIDevice {
public:

	/**
	 * @brief Construct a new MS560702BA03 device.
	 *
	 * Initializes the MS560702BA03 SPI driver interface and sets default
	 * configuration parameters for the IMU. The SPI interface, CS port,
	 * and pin are passed to the base `SPIDevice` class.
	 *
	 * @param data Pointer to the main DataContainer used for shared system data.
	 * @param spi Pointer to the SPI handle (SPI_HandleTypeDef).
	 * @param port Pointer to the GPIO port controlling the chip select (CS) pin.
	 * @param pin GPIO pin number used for chip select (CS).
	 */
	MS560702BA03(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

	/**
	 * @brief Initialize the MS560702BA03 device.
	 *
	 * Configures the IMU by writing default settings to key control registers,
	 * verifying communication with the device, and preparing it for operation.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	int deviceInit() override;

	/**
	 * @brief Perform periodic updates for the MS560702BA03 IMU.
	 *
	 * This method should be called regularly (e.g., in the main control loop)
	 * to read sensor data from the accelerometer and gyroscope, process it
	 * as needed, and update internal state or shared DataContainer values.
	 *
	 * Typical tasks performed by this method include:
	 *  - Reading raw accelerometer and gyroscope registers via SPI
	 *  - Converting raw readings into physical units using sensitivity constants
	 *  - Updating the main DataContainer with latest sensor values
	 *  - Performing basic filtering or status checks if required
	 *
	 * @return int Status code:
	 *  - 0: Update successful
	 *  - Negative value: Error reading data or SPI communication failure
	 *
	 * @note This function does not initialize the device; `deviceInit()` must
	 *       be called first.
	 */
	int updateDevice() override;

private:

	uint16_t C[7]; // Calibration coefficients

	uint32_t D1, D2;
	int32_t dT, TEMP, P;
	int64_t OFF, SENS;

	bool readProm();
	uint32_t readAdc(uint8_t cmd);

	enum MS5607_OSR  {
		OSR_256  = 0b000,
		OSR_512  = 0b001,
		OSR_1024 = 0b010,
		OSR_2048 = 0b011,
		OSR_4096 = 0b100
	} osr = OSR_256;

	int conversion_time;

	uint8_t buffer[3];
};

#endif /* SRC_DEVICES_SPI_DEVICES_MS560702BA03_H_ */

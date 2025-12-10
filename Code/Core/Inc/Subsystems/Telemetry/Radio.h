/*
 * Radio.h
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#ifndef SRC_SUBSYSTEMS_Radio_H_
#define SRC_SUBSYSTEMS_Radio_H_

#include <cstdint>
#include "stm32f4xx_hal.h"
#include "Subsystem.h"
extern "C"{
#include "rfm95.h"
}


/**
 * @class Radio
 * @brief Handles flight Radio logic.
 *
 * The Radio subsystem is responsible for sending telemetry from the rocket
 * to the ground station(s) and receiving commands from them to update
 * the vehicles status
 *
 * Typical tasks include:
 *  - Initializing Radio hardware
 *  - Receiving packets from ground station TODO
 */
class Radio: public Subsystem {
public:
	/**
	 * @brief Radio subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Radio(DataContainer* data, SPI_HandleTypeDef* _spi);


	/**
	 * @brief Initializes the Radio subsystem.
	 *
	 * This method sets up required hardware, allocates necessary buffers,
	 * and prepares the system for Radio updates.
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int init() override;

	/**
	 * @brief Allows the radio to poll for received data.
	 *
	 * This function should be called periodically (e.g., at a fixed update rate)
	 * to process new radio data
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;

	/**
	 * @brief Send data (eg. a telemetry packet) over the radio link.
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int send(uint8_t* data, int length);
private:
	SPI_HandleTypeDef* spi;
	rfm95_handle_t rfm95_handle;
};

#endif /* SRC_SUBSYSTEMS_Radio_H_ */

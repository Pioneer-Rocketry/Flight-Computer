/*
 * Radio.h
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#ifndef SRC_SUBSYSTEMS_Radio_H_
#define SRC_SUBSYSTEMS_Radio_H_

#include "Subsystem.h"

/**
 * @class Radio
 * @brief Handles flight Radio logic and sensor data fusion.
 *
 * The Radio subsystem is responsible sending telemetry from the rocket
 * to the ground station(s) and receiving commands from them to update
 * the vehicles status
 *
 * This class inherits from the base `Subsystem` interface and implements
 * the required `init()` and `update()` methods to manage its setup and
 * periodic updates within the main control loop.
 *
 * Typical tasks include:
 *  - Initializing Radio hardware
 *  - Assembling data packets
 *  - Sending packets at desired frequency
 *  - Receiving packets from ground station
 *  - Decoding and executing on what control packets contain
 */
class Radio: public Subsystem {
public:
	/**
	 * @brief Radio subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Radio(DataContainer* data);

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
	 * @brief Updates Radio data and state estimation.
	 *
	 * This function should be called periodically (e.g., at a fixed update rate)
	 * to process new radio data and send a telemetry packet if enabled and after
	 * timer
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;
};

#endif /* SRC_SUBSYSTEMS_Radio_H_ */

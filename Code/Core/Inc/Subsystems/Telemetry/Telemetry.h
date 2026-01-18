/*
 * Telemetry.h
 *
 *  Created on: Dec 2, 2025
 *      Author: Derek Prince
 */

#ifndef INC_SUBSYSTEMS_TELEMETRY_H_
#define INC_SUBSYSTEMS_TELEMETRY_H_

#include "Subsystem.h"
#include "main.h"

#include "Devices/SPI_Devices/RFM95.h"

#include <Subsystems/Telemetry/TelemetryPacketType.h>
#include "Radio.h"

/**
 * @class Telemetry
 * @brief Handles flight Telemetry logic
 *
 * The Telemetry subsystem is responsible for building telemetry packets to
 * send to the radio subsystem
 * to the ground station(s) and receiving commands from them to update
 * the vehicles status
 *
 * This class inherits from the base `Subsystem` interface and implements
 * the required `init()` and `update()` methods to manage its setup and
 * periodic updates within the main control loop.
 *
 * Tasks include:
 *  - Assembling data packets
 *  - Sending packets at desired frequency
 *  - Getting packets from radio TODO
 *  - Decoding and executing on what control packets contain TODO
 */

class Telemetry: public Subsystem {
public:

	/**
	 * @brief Radio subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Telemetry(DataContainer* data, SPI_HandleTypeDef* spiBus);

	/**
	 * @brief Initializes the Telemetry subsystem.
	 *
	 * This method sets up required hardware, allocates necessary buffers,
	 * and prepares the system for Radio updates.
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int init() override;

	/* Creates a new type of packet
	 * Transmitted every time interval
	 */
	TelemetryPacketType* createPacketType(uint32_t interval_ms);

	/**
	 * @brief Updates Radio data.
	 *
	 * This function should be called periodically (e.g., at a fixed update rate)
	 * to process new telemetry data and send a telemetry packet to the radio if
	 * enabled and after timer expires
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;

	int createBuffers();

private:
	RFM95 rfm95;

	// Radio radio;
	std::vector<TelemetryPacketType*> packetTypes;
};

#endif /* INC_SUBSYSTEMS_TELEMETRY_H_ */

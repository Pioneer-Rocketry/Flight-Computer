/*
 * Telemetry.h
 *
 *  Created on: Jan 9, 2026
 *      Author: Colin
 */

#ifndef INC_SUBSYSTEMS_TELEMETRY_H_
#define INC_SUBSYSTEMS_TELEMETRY_H_

#include "Subsystem.h"
#include "main.h"

#include "Devices/SPI_Devices/RFM95.h"

#define MAX_PAYLOAD_LENGTH 100

#define TRANSMISSION_FREQENCY 5.0f // hz
#define TRANSMISSION_INTERVAL 1000 / TRANSMISSION_FREQENCY // ms

#define TELEMETRY_PACKET_FIELDS(X) \
	X(float, GPSLatitude) \
	X(float, GPSLongitude) \
	X(float, GPSAltitude_m) \
	X(int, GPSFix) \
	X(int, GPSNumSatellites) \
	X(float, intergratedRoll) \
	X(float, KalmanFilterAltitude_m) \
	X(float, KalmanFilterVerticalVelocity_mps) \
	X(float, KalmanFilterVerticalAcceleration_mps2) \
	X(float, baroAltitudeOffset_m)

/**
 * @class Telemetry
 * @brief Handles flight Telemetry logic
 *
 * The Telemetry subsystem is responsible for building telemetry packets to
 * send to the radio to the ground station(s) and receiving commands from them
 * to update the vehicles status
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

private:
	RFM95 rfm95;

	union RadioPacket {
		struct {
			/* IMU Data*/
			#define x(type, name) type name;
			TELEMETRY_PACKET_FIELDS(x)
			#undef x
		} fields;
		uint8_t raw[MAX_PAYLOAD_LENGTH];
	} radioPacket;

    // Compile-time guard: fires if the struct exceeds the radio payload limit.
    static_assert(sizeof(radioPacket.fields) <= MAX_PAYLOAD_LENGTH,
                  "Telemetry packet exceeds MAX_PAYLOAD_LENGTH — "
                  "reduce fields or increase MAX_PAYLOAD_LENGTH.");

	uint32_t now;
	uint32_t lastTransmittion;

};

#endif /* INC_SUBSYSTEMS_TELEMETRY_H_ */

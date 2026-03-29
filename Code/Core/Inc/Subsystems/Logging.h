/*
 * Navigation.h
 *
 *  Created on: Feb 17, 2026
 *      Author: Derek Prince
 */

#ifndef SRC_SUBSYSTEMS_LOGGING_H_
#define SRC_SUBSYSTEMS_LOGGING_H_


#include "Subsystem.h"
#include "main.h"
#include <cstdint>
#include <vector>
//#include "utils.h"
//#include "defines.h"
//#include "Devices/Flash.h"
#include "Devices/SPI_Devices/W25Q128JV.h"

#define LOG_PAGE_BUFFER_SIZE_BYTES W25Q128JV_PAGE_SIZE - 1 // Using the first byte of a page to keep track of if there is data

#define MAX_PACKET_LENGTH 254

#define TRANSMISSION_FREQENCY 1/10.0f // hz
#define TRANSMISSION_INTERVAL 10000 // ms

#define LOGGING_PACKET_FIELDS(X) \
	X(float, LSM6DSV320GyroX_dps) \
	X(float, LSM6DSV320GyroY_dps) \
	X(float, LSM6DSV320GyroZ_dps) \
	X(float, LSM6DSV320LowGAccelX_mps2) \
	X(float, LSM6DSV320LowGAccelY_mps2) \
	X(float, LSM6DSV320LowGAccelZ_mps2) \
	X(float, LSM6DSV320HighGAccelX_mps2) \
	X(float, LSM6DSV320HighGAccelY_mps2) \
	X(float, LSM6DSV320HighGAccelZ_mps2) \
	X(float, MS560702BA03Temperature_C) \
	X(float, MS560702BA03Pressure_hPA) \
	X(float, MS560702BA03Altitude_m) \
	X(float, GPSLatitude) \
	X(float, GPSLongitude) \
	X(float, GPSAltitude_m) \
	X(int, GPSFix) \
	X(int, GPSNumSatellites)

class Logging: public Subsystem
{
    public:
	bool enableWriting = true;
    Logging(DataContainer* data, SPI_HandleTypeDef* _spi);

    int init() override;
    int update() override;

    private:
    W25Q128JV flash;
	uint16_t currentPage = 0;
	uint16_t currentSector = 0;

    bool loggingEnabled = false;

	union FlashPacket {
		struct {
			/* IMU Data*/
			#define x(type, name) type name;
			LOGGING_PACKET_FIELDS(x)
			#undef x
		} fields;
		uint8_t raw[MAX_PACKET_LENGTH];
	} flashPacket;

	// Compile-time guard: fires if the struct exceeds the radio payload limit.
    static_assert(sizeof(flashPacket.fields) <= MAX_PACKET_LENGTH,
                  "Flash packet exceeds MAX_PACKET_LENGTH — "
                  "reduce fields or allow for writing to more than one page per write.");

	uint32_t now;
	uint32_t lastLog;
};



#endif /* SRC_SUBSYSTEMS_LOGGING_H_ */

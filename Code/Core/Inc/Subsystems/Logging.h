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
#include "usbHelper.h"
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

	void dumpFlash(){
		uint8_t buffer[256];
		for (uint16_t page = 0; page <= 0xFFFF; page++){ //Read one page at a time
			flash.readData(((uint32_t)page) << 8, buffer, 0xFF);
			char pageAddrStringBuffer[4]; //aaaa //16bit page address
			sprintf(pageAddrStringBuffer, "%04X", page);

			for (uint8_t line = 0x0; line < 0xF; line++){//For print 16 bytes each of the lines, so 16 per page 
				cdcSendMessage(pageAddrStringBuffer, 8);

				char stringBuffer[55]; //54 bytes of chars, 1 for newline format "aaaa: BB BB BB BB BB BB BB BB  BB BB BB BB BB BB BB BB"
				
				sprintf(stringBuffer, "%X0: ", line); //Last byte of address, placeholder 4 bits as 16 bytes are represented per line
				for (uint8_t byteInLine = 0; byteInLine < 8; byteInLine++){

							// ptr to string buffer, format, source
							// 3 chars per byte, 4 bytes for "aa: "
					sprintf(stringBuffer + (byteInLine * 3) + 4, "%02X", buffer[line*16+byteInLine]);

					stringBuffer[2+byteInLine] = ' ';
				}

				//Extra space between sets
				//3 chars per byte (8 done already), 4 for "aa: "
				stringBuffer[3*8+4] = ' ';
				
				//Same as above to second set of bytes on the line
				for (uint8_t byteInLine = 8; byteInLine < 16; byteInLine++){
					//Same as above, but extra +1 for extra space character between sets
					sprintf(stringBuffer + (byteInLine * 3) + 4 + 1, "%02X", buffer[line*16+byteInLine]);
					stringBuffer[2+byteInLine+1] = ' ';
				}
				//Set the last char as a newline
				stringBuffer[54] = '\n';

				//Print the line
				cdcSendMessage(stringBuffer, 3);
			}
		}
	}

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

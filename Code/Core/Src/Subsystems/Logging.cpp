/*
 * Navigation.cpp
 *
 *  Created on: Feb 18, 2026
 *      Author: Derek Prince
 */

#include "Subsystems/Logging.h"
#include "usbHelper.h"

Logging::Logging(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	flash(spiBus, FLASH_CS_GPIO_Port, FLASH_CS_Pin)
{

}

int Logging::init()
{
	flash.init();
	//Read first byte of each page to see if it has data, set current page to next
	uint32_t i = 0;
	uint8_t readByte = 0xFF;
	do {
		readByte = flash.readByte(i += W25Q128JV_PAGE_SIZE);

		if (i > 65535){
			return -1; // Flash is full
		}


	} while (readByte != 0xFF && i < W25Q128JV_PAGE_SIZE*W25Q128JV_PAGE_COUNT); // Empty flash is filled with 0xFF

	currentPage = i; //Choose first unfilled page
	//First byte of each packet is the packet ID, which start at 1 and go up, so if
	currentSector = currentPage >> 4; //16 pages per sector

	return 0;
}

int Logging::update()
{

	now = HAL_GetTick();

    if (now - lastLog >= TRANSMISSION_INTERVAL)
    {
		#define x(type, name) flashPacket.fields.name = data->name;
		LOGGING_PACKET_FIELDS(x)
		#undef x

		if (currentPage << 4 != currentSector){ //Moved into next sector
			flash.eraseSector(currentPage << 4);
			currentSector = currentPage << 4;
		}

		if (enableWriting){
			flash.writePage(currentPage, flashPacket.raw, sizeof(flashPacket.raw));
		}

		currentPage++;
		if (currentPage > 2^16){
			enableWriting = false;
		}

        lastLog = HAL_GetTick();
    }

	return 0;
}

void Logging::dumpFlash(){
	uint8_t buffer[256];
	for (uint32_t page = 0; page <= 0xFFFF; page++){ //Read one page at a time
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
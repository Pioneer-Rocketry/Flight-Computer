/*
 * Navigation.cpp
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#include "Subsystems/Telemetry.h"
#include <stdio.h>


/*	Implements something like:
 * 	Packet type list
 * 		Each element is a type of packet (high rate telem, low rate telem, state)
 * 			Each type of packet has a frequency and data elements it contains
 * 	Each type is made by calling CreatePacketType and passing tx interval
 * 	Then calling packet.type.addPacketElement(data* variable, size) to set the packet to
 * 		pull data from that pointer, probably to a dataContainer variable
 */

Telemetry::Telemetry(DataContainer* data, SPI_HandleTypeDef* spiBus)
	: Subsystem(data),
	  rfm95(data, spiBus, LORA_CS_GPIO_Port, LORA_CS_Pin)
{
}

int Telemetry::init()
{
	rfm95.deviceInit();

	return 0;
}

int Telemetry::update()
{
	rfm95.updateDevice();

	return 0;
}

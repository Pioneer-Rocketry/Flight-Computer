/*
 * Navigation.cpp
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#include "Subsystems/Telemetry/Telemetry.h"

/*	Implements something like:
 * 	Packet type list
 * 		Each element is a type of packet (high rate telem, low rate telem, state)
 * 			Each type of packet has a frequency and data elements it contains
 * 	Each type is made by calling CreatePacketType and passing tx interval
 * 	Then calling packet.type.addPacketElement(data* variable, size) to set the packet to
 * 		pull data from that pointer, probably to a dataContainer variable
 */

Telemetry::Telemetry(DataContainer* data, Radio _radio) : Subsystem(data), radio(_radio)
{
}

//Create a new type of packet
//Returns the new packet type object to allow for its initialization of adding data sources
TelemetryPacketType* Telemetry::CreatePacketType(uint32_t interval_ms){
	TelemetryPacketType* newPacketType = new TelemetryPacketType(interval_ms);
	newPacketType->packetID = packetTypes.size();
	packetTypes.push_back(newPacketType);
	return newPacketType;
}

int Telemetry::init()
{
	return 0;
}

int Telemetry::update()
{
	for (TelemetryPacketType* packetType: packetTypes){
		if (HAL_GetTick() >= packetType->globalTimeToSendNext_ms){

			uint8_t * packetBuffer;
			size_t size = packetType->getPacketSize();

			if ((packetBuffer = (uint8_t*)malloc(size)) != nullptr){
				packetType->collectData(packetBuffer, size);
			}

			radio.send(packetBuffer, size);

			free(packetBuffer);

			packetType->globalTimeToSendNext_ms = HAL_GetTick() + packetType->sendInterval_ms;
		}
	}

	return 0;
}

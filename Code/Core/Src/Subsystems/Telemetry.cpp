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
	  rfm95(data, spiBus, LORA_CS_GPIO_Port, LORA_CS_Pin, MAX_PAYLOAD_LENGTH)
{
}

int Telemetry::init()
{
	rfm95.init();

    lastTransmittion = HAL_GetTick();

	return 0;
}

int Telemetry::update()
{
	now = HAL_GetTick();

    if (now - lastTransmittion >= TRANSMISSION_INTERVAL)
    {
		radioPacket.fields.LSM6DSV320GyroX_dps			= data->LSM6DSV320GyroX_dps;
		radioPacket.fields.LSM6DSV320GyroY_dps			= data->LSM6DSV320GyroY_dps;
		radioPacket.fields.LSM6DSV320GyroZ_dps			= data->LSM6DSV320GyroZ_dps;
		radioPacket.fields.LSM6DSV320LowGAccelX_mps2	= data->LSM6DSV320LowGAccelX_mps2;
		radioPacket.fields.LSM6DSV320LowGAccelY_mps2	= data->LSM6DSV320LowGAccelY_mps2;
		radioPacket.fields.LSM6DSV320LowGAccelZ_mps2	= data->LSM6DSV320LowGAccelZ_mps2;
		radioPacket.fields.LSM6DSV320HighGAccelX_mps2	= data->LSM6DSV320HighGAccelX_mps2;
		radioPacket.fields.LSM6DSV320HighGAccelY_mps2	= data->LSM6DSV320HighGAccelY_mps2;
		radioPacket.fields.LSM6DSV320HighGAccelZ_mps2	= data->LSM6DSV320HighGAccelZ_mps2;
		radioPacket.fields.MS560702BA03Temperature_C	= data->MS560702BA03Temperature_C;
		radioPacket.fields.MS560702BA03Pressure_hPA		= data->MS560702BA03Pressure_hPA;
		radioPacket.fields.MS560702BA03Altitude_m		= data->MS560702BA03Altitude_m;
		radioPacket.fields.GPSLatitude					= data->GPSLatitude;
		radioPacket.fields.GPSLongitude					= data->GPSLongitude;
		radioPacket.fields.GPSAltitude_m				= data->GPSAltitude_m;
		radioPacket.fields.GPSFix						= data->GPSFix;
		radioPacket.fields.GPSNumSatellites				= data->GPSNumSatellites;
		radioPacket.fields.GPSUTCTime[16]				= data->GPSUTCTime[16];

		rfm95.sendPacket(radioPacket.raw, sizeof(radioPacket.fields));

        lastTransmittion = HAL_GetTick();
    }

	return 0;
}

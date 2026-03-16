/*
 * Navigation.cpp
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#include "Subsystems/Telemetry.h"
#include <stdio.h>


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
		#define x(type, name) radioPacket.fields.name = data->name;
		TELEMETRY_PACKET_FIELDS(x)
		#undef x

		rfm95.sendPacket(radioPacket.raw, sizeof(radioPacket.fields));

        lastTransmittion = HAL_GetTick();
    }

	return 0;
}

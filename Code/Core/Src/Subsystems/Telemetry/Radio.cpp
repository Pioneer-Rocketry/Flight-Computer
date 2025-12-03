/*
 * Navigation.cpp
 *
 *  Created on: Oct 31, 2025
 *      Author: Derek Prince
 */

#include <stdio.h>
#include "main.h"
#include "Subsystems/Telemetry/Radio.h"

Radio::Radio(DataContainer* data, SPI_HandleTypeDef _spi) : Subsystem(data)
{
	spi = _spi;
}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RFM95_DIO0_Pin) {
		rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO0);
	} else if (GPIO_Pin == RFM95_DIO1_Pin) {
		rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO1);
	} else if (GPIO_Pin == RFM95_DIO5_Pin) {
		rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO5);
	}
};*/

int Radio::init()
{
	// Create the handle for the RFM95 module.
	rfm95_handle_t _rfm95_handle = {
		.spi_handle = &spi,
		.nss_port = LORA_CS_GPIO_Port,
		.nss_pin = LORA_CS_Pin,
		.nrst_port = LORA_RESET_GPIO_Port,
		.nrst_pin = LORA_RESET_Pin,
		.device_address = {
			0x00, 0x00, 0x00, 0x00
		},
		.network_session_key = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		},
		.application_session_key = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		},
		.receive_mode = RFM95_RECEIVE_MODE_RX12
	};

	rfm95_handle = _rfm95_handle;

	// Initialize RFM95 module.
	if (!rfm95_init(&rfm95_handle)) {
		printf("RFM95 init failed\n\r");
		return -1;
	};

	uint8_t data_packet[] = {
		0x01, 0x02, 0x03, 0x4
	};

	if (!rfm95_send_receive_cycle(&rfm95_handle, data_packet, sizeof(data_packet))) {
		printf("RFM95 send failed\n\r");
	} else {
		printf("RFM95 send success\n\r");
	}

	return 0;
}


int Radio::update()
{
	return 0;
}

int Radio::send(uint8_t* data, int length){
	if (!rfm95_send_receive_cycle(&rfm95_handle, data, sizeof(length))) {
		printf("RFM95 send failed\n\r");
		return -1; //Send Error
	}
	return 0;
}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RFM95_DIO0_Pin) {
        rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO0);
    } else if (GPIO_Pin == RFM95_DIO1_Pin) {
        rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO1);
    } else if (GPIO_Pin == RFM95_DIO5_Pin) {
        rfm95_on_interrupt(&rfm95_handle, RFM95_INTERRUPT_DIO5);
    }
}
*/

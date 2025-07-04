#ifndef RADIO_RFM95W_H
#define RADIO_RFM95W_H

#include "stm32f4xx_hal.h" /* Needed for SPI */

#include "SPI_Devices/SPI_Device.hpp"
#include "data.h"

// Registers
#define RFM95W_FIFO					0x00
#define RFM95W_OP_MODE				0x01
#define RFM95W_FR_MSB				0x06
#define RFM95W_FR_MID				0x07
#define RFM95W_FR_LSB				0x08
#define RFM95W_PA_CONFIG			0x09
#define RFM95W_PA_RAMP				0x0A
#define RFM95W_OCP					0x0B
#define RFM95W_LNA					0x0C
#define RFM95W_FIFO_ADDR_PTR		0x0D
#define RFM95W_FIFO_TX_BASE_ADDR	0x0E
#define RFM95W_FIFO_RX_BASE_ADDR	0x0F
#define RFM95W_IRQ_FLAGS	 		0x10
#define RFM95W_IRQ_MASK 			0x11
#define RFM95W_FREQ_IF_MSB 			0x12
#define RFM95W_FREQ_IF_LSB 			0x13
#define RFM95W_SYMB_TIMEOUT_MSB 	0x14
#define RFM95W_SYMB_TIMEOUT_LSB 	0x15
#define RFM95W_TX_CFG 				0x16
#define RFM95W_PAYLOAD_LENGTH 		0x17
#define RFM95W_PREAMBLE_MSB 		0x18
#define RFM95W_PREAMBLE_LSB 		0x19
#define RFM95W_MODULATION_CFG 		0x1A
#define RFM95W_RF_MODE 				0x1B
#define RFM95W_HOP_PERIOD 			0x1C
#define RFM95W_MODEM_CONFIG1	 	0x1D
#define RFM95W_MODEM_CONFIG2	 	0x1E
#define RFM95W_SYMB_TIMEOUT_L		0x1F
#define RFM95W_RX_PACKET_CNT_VALUE	0x20
#define RFM95W_MODEM_STAT 			0x21
#define RFM95W_PKT_SNR_VALUE 		0x22
#define RFM95W_RSSI_VALUE 			0x23
#define RFM95W_PKT_RSSI_VALUE 		0x24
#define RFM95W_HOP_CHANNEL 			0x25
#define RFM95W_MODEM_CONFIG3 		0x26
#define RFM95W_DIO_MAPPING_1 		0x40
#define RFM95W_DIO_MAPPING_2 		0x41
#define RFM95W_VERSION 				0x42
#define RFM95W_TCXO 				0x43
#define RFM95W_PA_DAC 				0x4B
#define RFM95W_FORMER_TEMP 			0x58
#define RFM95W_AGC_REG 				0x61
#define RFM95W_AGC_THRESH_1 		0x62
#define RFM95W_AGC_THRESH_2 		0x63
#define RFM95W_AGC_THRESH_3			0x64


class Radio_RFM95W : public SPI_Device
{
private:

	int64_t frequency = 915000000;
	int32_t frf;

public:
    Radio_RFM95W(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin, int64_t frequency);

    bool begin() override;
    void loop() override;
};

#endif /* RADIO_RFM95W_H */

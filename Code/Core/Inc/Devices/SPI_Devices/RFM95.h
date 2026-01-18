/*
 * RFM95.h
 *
 *  Created on: Jan 9, 2026
 *      Author: colin
 */

#ifndef SRC_DEVICES_SPI_DEVICES_RFM95_H_
#define SRC_DEVICES_SPI_DEVICES_RFM95_H_


#include "Devices/SPIDevice.h"

#define RFM95_FIFO 						0x00
#define RFM95_OP_MODE 					0x01
#define RFM95_FR_MSB 					0x06
#define RFM95_FR_MID 					0x07
#define RFM95_FR_LSB 					0x08
#define RFM95_PA_CONFIG 				0x09
#define RFM95_PA_RAMP 					0x0A
#define RFM95_OCP 						0x0B
#define RFM95_LNA 						0x0C
#define RFM95_FIFO_ADDR_PTR 			0x0D
#define RFM95_FIFO_TX_BASE_ADDR			0x0E
#define RFM95_FIFO_RX_BASE_ADDR			0x0F
#define RFM95_FIFO_RX_CURRENT_ADDR		0x10
#define RFM95_IRQ_FLAGS_MASK 			0x11
#define RFM95_IRQ_FLAGS 				0x12
#define RFM95_RX_NB_BYTES 				0x13
#define RFM95_RX_HEADER_CNT_VALUE_MSB 	0x14
#define RFM95_RX_HEADER_CNT_VALUE_LSB 	0x15
#define RFM95_RX_PACKET_CNT_VALUE_MSB	0x16
#define RFM95_RX_PACKET_CNT_VALUE_LSB	0x17
#define RFM95_MODEM_STAT 				0x18
#define RFM95_PKT_SNR_VALUE				0x19
#define RFM95_PKT_RSSI_VALUE 			0x1A
#define RFM95_RSSI_VALE					0x1B
#define RFM95_HOP_CHANNEL 				0x1C
#define RFM95_MODEM_CONFIG 				0x1D
#define RFM95_MODEM_CONFIG_2 			0x1E
#define RFM95_SYMB_TIMEOUT_LSB	 		0x1F
#define RFM95_PREAMBLE_MSB		 		0x20
#define RFM95_PREAMBLE_LSB 				0x21
#define RFM95_PAYLOAD_LENGTH 			0x22
#define RFM95_MAX_PAYLOAD_LENGTH		0x23
#define RFM95_HOP_PERIOD				0x24
#define RFM95_FIFO_RX_BYTES_ADDR		0x25
#define RFM95_MODEM_CONFIG_3			0x26

#define RFM95_DIO_MAPPING_1 			0x40
#define RFM95_DIO_MAPPING_2 			0x41
#define RFM95_VERSION 					0x42
#define RFM95_TCX0 						0x4B
#define RFM95_PA_DAC 					0x4D
#define RFM95_FORMER_TEMP 				0x5B
#define RFM95_AGC_REF 					0x61
#define RFM95_AGC_THRESH_1 				0x62
#define RFM95_AGC_THRESH_2 				0x63
#define RFM95_AGC_THRESH_3 				0x64

#define RFM95_FXOSC 32000000UL
#define RFM95_FSTEP (RFM95_FXOSC / (1UL << 19))

#define MAX_PAYLOAD_LENGTH 100

#define TRANSMISSION_FREQENCY 1/10.0f // hz
#define TRANSMISSION_INTERVAL 10000 // ms

/**
 * @class RFM95
 * @brief Driver for the RFM95 LoRa Radio.
 *
 * This class implements an SPI-based driver for the RFM95 LoRa Radio module.
 */
class RFM95: public SPIDevice {
public:

	/**
	 * @brief Construct a new RFM95 device.
	 *
	 *
	 * @param data Pointer to the main DataContainer used for shared system data.
	 * @param spi Pointer to the SPI handle (SPI_HandleTypeDef).
	 * @param port Pointer to the GPIO port controlling the chip select (CS) pin.
	 * @param pin GPIO pin number used for chip select (CS).
	 */
	RFM95(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

	/**
	 * @brief Initialize the RFM95 device.
	 *
     *
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	int deviceInit() override;

	/**
	 * @brief Perform periodic updates for the RFM95 IMU.
	 *
	 * @return int Status code:
	 *  - 0: Update successful
	 *  - Negative value: Error reading data or SPI communication failure
	 *
	 * @note This function does not initialize the device; `deviceInit()` must
	 *       be called first.
	 */
	int updateDevice() override;


private:

	void setFreq(uint32_t freqHz);

	void compilePacket();
	void sendPacket();

	// LoRa Calculator
	// https://www.semtech.com/design-support/lora-calculator

	enum RFM95_BANDWIDTH {
		BANDWITDH_7kHz8 	= 0b0000,
		BANDWITDH_10kHz4 	= 0b0001,
		BANDWITDH_15kHz6 	= 0b0010,
		BANDWITDH_20kHz8 	= 0b0011,
		BANDWITDH_31kHz25 	= 0b0100,
		BANDWITDH_41kHz7 	= 0b0101,
		BANDWITDH_62kHz5 	= 0b0110,
		BANDWITDH_125kHz 	= 0b0111,
		BANDWITDH_250kHz 	= 0b1000,
		BANDWITDH_500kHz 	= 0b1001
	} bandwidth = BANDWITDH_31kHz25;

	enum RFM95_CODING {
		CODING_4_5 	= 0b001,
		CODING_4_6 	= 0b010,
		CODING_4_7 	= 0b011,
		CODING_4_8 	= 0b100
	} coding = CODING_4_5;

	enum RFM95_SPREADFACTOR {
		SF_6	= 6,
		SF_7	= 7,
		SF_8	= 8,
		SF_9	= 9,
		SF_10	= 10,
		SF_11	= 11,
		SF_12	= 12
	} spreadfactor = SF_10;

	// uint16_t rxTimeout = ;

	uint32_t frf;
	uint8_t frfMSB;
	uint8_t frfMID;
	uint8_t frfLSB;

	uint8_t read;
	uint8_t command;

	uint32_t now;
	uint32_t lastTransmittion;

	uint8_t payload[MAX_PAYLOAD_LENGTH];
	uint8_t payloadLength;

};

#endif /* SRC_DEVICES_SPI_DEVICES_RFM95_H_ */
#include "SPI_Devices/radio_RFM95W.h"

Radio_RFM95W::Radio_RFM95W(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin, int64_t frequency)
    : SPI_Device(data, spi, port, pin, "RFM95W") {

	this->frequency = frequency;
	this->frf = (double)frequency / 61.03515625;
}

bool Radio_RFM95W::begin() {

    /* Radio Configuration */
    uint8_t config = 0;

	// Put device in sleep mode

	// OP_MODE page 87
	config  = 0;
	config |= 0b1   << 7; // Enable LoRa Mode
	config |= 0b0   << 6; // Access to LoRa register page
	config |= 0b00  << 4; // Reserved
	config |= 0b0   << 3; // High Frequency Mode
	config |= 0b000 << 0; // Sleep Mode
	write_SPI(RFM95W_OP_MODE, &config);

	// OP_MODE page 87
	config  = 0;
	config |= 0b1   << 7; // Enable LoRa Mode
	config |= 0b0   << 6; // Access to LoRa register page
	config |= 0b00  << 4; // Reserved
	config |= 0b0   << 3; // High Frequency Mode
	config |= 0b000 << 0; // Sleep Mode
	write_SPI(RFM95W_OP_MODE, &config);

	// FR_MSB
	config = frf >> 16;
	write_SPI(RFM95W_FR_MSB, &config);

	// FR_MID
	config = frf >> 8;
	write_SPI(RFM95W_FR_MID, &config);

	// FR_LSB
	config = frf >> 0;
	write_SPI(RFM95W_FR_LSB, &config);

	// PA_CONFIG
	config  = 0;
	config |= 0b1    << 7; // Enabled PA_BOOST
	config |= 0b111  << 4; // Set max power
	config |= 0b1111 << 0; // Set output power
	write_SPI(RFM95W_PA_CONFIG, &config);

	// PA_RAMP
	config  = 0;
	config |= 0b0000 << 3; // Reserved
	config |= 0b1001 << 0; // PA Ramp Time

	write_SPI(RFM95W_PA_RAMP, &config);

	// OCP
	config  = 0;
	config |= 0b00 	  << 6; // Unused
	config |= 0b0 	  << 5; // Turn off OCP
	config |= 0b11111 << 0; // OCP Trim
	write_SPI(RFM95W_OCP, &config);

	// LNA
	config  = 0;
	config |= 0b001	<< 5; // Set LNA Gain to Max
	config |= 0b00 	<< 3; // Default LNA current
	config |= 0b00 	<< 1; // Reserved
	config |= 0b11 	<< 0; // Enable boost
	write_SPI(RFM95W_LNA, &config);


	// Put device in stand by mode

	// OP_MODE page 87
	config  = 0;
	config |= 0b1   << 7; // Enable LoRa Mode
	config |= 0b0   << 6; // Access to LoRa register page
	config |= 0b00  << 4; // Reserved
	config |= 0b0   << 3; // High Frequency Mode
	config |= 0b001 << 0; // Sleep Mode
	write_SPI(RFM95W_OP_MODE, &config);

	// MODEM_CONFIG1
	config  = 0;
	config |= 0b0110 << 4; // 62.5 kHz bandwidth
	config |= 0b001  << 1; // 4/5 coding
	config |= 0b1    << 0; // Implicit header
	write_SPI(RFM95W_MODEM_CONFIG1, &config);

	// MODEM_CONFIG2
	config  = 0;
	config |= 0b1111 << 4; // 12 Spread Factor
	config |= 0b1    << 3; // Enabled TX Continuous Mode
	config |= 0b0    << 2; // Turn off CRC on RX packets
	config |= 0b00   << 0; // Default value to timeout
	write_SPI(RFM95W_MODEM_CONFIG2, &config);

	// SYMB_TIMEOUT_L
	config = 0x64; // Default value
	write_SPI(RFM95W_SYMB_TIMEOUT_L, &config);

	// MODEM_CONFIG3
	config  = 0;
	config |= 0b0000 << 4; // Unused
	config |= 0b1    << 3; // Set it to mobile node
	config |= 0b0    << 2; // Set Register LNA Gain
	config |= 0b00   << 0; // Reserved
	write_SPI(RFM95W_MODEM_CONFIG3, &config);

    return true;
}

void Radio_RFM95W::loop() {
}
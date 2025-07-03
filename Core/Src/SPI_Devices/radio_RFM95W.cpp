#include "SPI_Devices/radio_RFM95W.h"

Radio_RFM95W::Radio_RFM95W(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin, float frequency)
    : SPI_Device(data, spi, port, pin, "RFM95W") {

	this->frequency = frequency;
}

bool Radio_RFM95W::begin() {

    /* Radio Configuration */
    uint8_t config = 0;

	// OP_MODE page 87
	config  = 0;
	config |= 0b1   << 7; // Enable LoRa Mode
	config |= 0b0   << 6; // Access to LoRa register page
	config |= 0b00  << 4; // Reserved
	config |= 0b0   << 3; // High Frequency Mode
	config |= 0b000 << 0; // Sleep Mode
	write_SPI(RFM95W_OP_MODE, &config);

	// FR_MSB

	// FR_MID

	// FR_LSB

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

    return true;
}

void Radio_RFM95W::loop() {
}
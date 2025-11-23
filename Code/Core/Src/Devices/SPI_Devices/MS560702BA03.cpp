/*
 * MS560702BA03.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/SPI_Devices/MS560702BA03.h"

MS560702BA03::MS560702BA03(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
	: SPIDevice(data, spi, port, pin)
{
	data->MS560702BA03Temperature = 0.0f;
	data->MS560702BA03Pressure = 0.0f;
	data->MS560702BA03Altitude = 0.0f;

}

int MS560702BA03::deviceInit()
{
	if (writeSPI(MS5607_RESET, nullptr, 0) != HAL_OK) {
		return false;
	}

	HAL_Delay(3); // Max reset time

	switch (osr) {
		case OSR_256:   conversion_time = 1; break;
		case OSR_512:   conversion_time = 1; break;
		case OSR_1024:  conversion_time = 2; break;
		case OSR_2048:  conversion_time = 5; break;
		case OSR_4096:  conversion_time = 9; break;
		default: return false;
	}

	return readProm();
}

int MS560702BA03::readProm() {
    for (uint8_t i = 0; i < 7; i++) {
        if (readSPI(MS5607_PROM_READ + (i * 2), buffer, 2) != HAL_OK)
            return -1;
        C[i] = (buffer[0] << 8) | buffer[1];
    }
    return 0;
}

uint32_t MS560702BA03::readADC(uint8_t cmd) {
    writeSPI(cmd, nullptr, 0);
    HAL_Delay(conversion_time);

    readSPI(MS5607_ADC_READ, buffer, 3);

    return (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
}

int MS560702BA03::updateDevice()
{
	D1 = readADC(MS5607_CONVERT_D1 | (osr << 1));
	D2 = readADC(MS5607_CONVERT_D2 | (osr << 1));

	dT = D2 - ((uint32_t)C[5] << 8);
	TEMP = 2000 + ((int64_t)dT * C[6]) / (1 << 23);

	OFF = ((int64_t)C[2] << 17) + (((int64_t)C[4] * dT) >> 6);
	SENS = ((int64_t)C[1] << 16) + (((int64_t)C[3] * dT) >> 7);

	P = (((D1 * SENS) >> 21) - OFF) >> 15;

	this->data->MS560702BA03Temperature = TEMP / 100.0f;
	this->data->MS560702BA03Pressure = P / 100.0f;
	this->data->MS560702BA03Altitude = (1-powf(this->data->MS560702BA03Pressure/1013.25, 0.190284))*145366.45; // * FT_TO_M;

	return 0;
}

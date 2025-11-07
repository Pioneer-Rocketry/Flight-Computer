/*
 * LSM6DSV320.cpp
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#include "Devices/SPI_Devices/LSM6DSV320.h"

LSM6DSV320::LSM6DSV320(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
	: SPIDevice(data, spi, port, pin)
{

}

int LSM6DSV320::deviceInit()
{
	uint8_t whoAmI = 0;

	readSPI(LSM6DSV320_WHO_AM_I, &whoAmI);
	if (whoAmI != LSM6DSV320_ID) {
		return -1;
	}

	/* Sensor Configuration */
	uint8_t config = 0;
	uint8_t out[2] = {0,0};

	// IF_CFG page 58, table 32
	config  = 0;
	config |= 0 << 7; // SDA_PU_EN disconnects the SDA pull-up (default)
	config |= 0 << 6; // SHUB_PU_EN Disables I2C plus pull-ups (default)
	config |= 0 << 5; // ASF_CTRL turns off anti-spike filter (default)
	config |= 0 << 4; // H_LACTIVE sets interrupts to active high (default)
	config |= 0 << 3; // PP_OD sets interrupts to push-pull (default)
	config |= 0 << 2; // SIM sets the SPI interface more to 4 wire (default)
	config |= 0 << 1; // Reserved
	config |= 0 << 0; // I2C_I3C_DISABLE disables I2C and I3C interface (default)
	writeSPI(LSM6DSV320_IF_CFG, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_IF_CFG, out);

	HAL_Delay(1);

	// CTRL1 page 65, table 52
	config  = 0;
	config |= this->lowGOPMode << 4; // OP_MODE_XL
	config |= this->lowGODR    << 0; // ODR_XL
	writeSPI(LSM6DSV320_CTRL1, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL1, out);

	HAL_Delay(1);

	// CTRL2 page 66, table 55
	config  = 0;
	config |= this->gyroOPMode << 4; // OP_MODE_G
	config |= this->gyroODR    << 0; // ODR_G
	writeSPI(LSM6DSV320_CTRL2, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL2, out);

	HAL_Delay(1);

	// CTRL3 page 67, table 58
	config  = 0;
	config |= 0b0 << 7; // BOOT set to normal mode
	config |= 0b1 << 6; // BDU set to
	config |= 0b1 << 2; // IF_INC set to
	config |= 0b0 << 0; // SW_RESET set to normal mode
	writeSPI(LSM6DSV320_CTRL3, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL3, out);

	HAL_Delay(1);

	// CTRL4 page 68, table 60
	config  = 0;
	config |= 0b0 << 4; // INT2_on_INT1
	config |= 0b0 << 3; // DRDY_MASK set to disabled
	config |= 0b0 << 2; // INT2_DRDY_TEMP set to disabled
	config |= 0b0 << 1; // DRDY_PULSED set data-ready latched mode
	config |= 0b0 << 0; // INT2_IN_LH set embedded functions active low
	writeSPI(LSM6DSV320_CTRL4, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL4, out);

	HAL_Delay(1);

	// CTRL5 page 69, table 62
	config  = 0;
	config |= 0b0  << 7; // IF2_TA0_PID
	config |= 0b00 << 2; // BUS_ACT_SEC sets the bus available time selection for IBT to 50 us (default)
	config |= 0b0  << 0; // INT_EN_SPI disables INT pin when I3C is enabled (default)
	writeSPI(LSM6DSV320_CTRL5, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL5, out);

	HAL_Delay(1);

	// CTRL6 page 69, table 64
	config  = 0;
	config |= 0b110           << 4; // LPF1_G_BW sets the low-pass filter for gyroscope bandwidth selection
	config |= this->gyroRange << 0; // FS_G
	writeSPI(LSM6DSV320_CTRL6, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL6, out);

	HAL_Delay(1);

	// CTRL7 page 70, table 67
	config  = 0;
	config |= 0b0 << 7; // INT1_DRDY_XL disabled high-g data-ready interrupt on INT1 pin (default)
	config |= 0b0 << 6; // INT2_DRDY_XL disables high-g data-ready interrupt on INT2 pin (default)
	config |= 0b1 << 0; // LPF1_G_EN enables low-pass filter for gyroscope (default)
	writeSPI(LSM6DSV320_CTRL7, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL7, out);

	HAL_Delay(1);

	// CTRL8 page 71, table 69
	config  = 0;
	config |= 0b000           << 5; // HP_LPF2_XL_BW set to ODR / 4
	config |= this->lowGRange << 0; // FS_XL
	writeSPI(LSM6DSV320_CTRL8, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL8, out);

	HAL_Delay(1);

	// CTRL9 page 72, table 72
	config  = 0;
	config |= 0b0 << 6; // HP_REF_MODE_XL disables the high-pass filter reference mode (default)
	config |= 0b0 << 5; // XL_FASTSETTL_MODE disables the fast-settling mode (default)
	config |= 0b0 << 4; // HP_SLOPE_XL_EN selects low-pass filter slope (default)
	config |= 0b1 << 3; // LPF2_XL_EN
	config |= 0b0 << 1; // USR_OFF_W sets user offsets to be 2^-10 g/LSB (default)
	config |= 0b0 << 0; // USR_OFF_ON_OUT bypasses user offset correction block (default)
	writeSPI(LSM6DSV320_CTRL9, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL9, out);

	HAL_Delay(1);

	// CTRL10 page 74, table 74
	config  = 0;
	config |= 0b0  << 6; // EMB_FUNC_DEBUG disables the embedded functions
	config |= 0b00 << 2; // ST_G sets the normal self-test mode (default)
	config |= 0b00 << 0; // ST_XL sets the normal self-test mode (default)
	writeSPI(LSM6DSV320_CTRL10, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL10, out);

	// CTRL1_XL_HG page 91, table 147
	config  = 0;
	config |= 0b1              << 7; // XL_HG_REGOUT_EN enable high-g outputs
	config |= 0b0              << 6; // HG_USR_OUFF_OU_OUT disabled high-g offsets
	config |= this->highGODR   << 3; // ODR_XL_HG
	config |= this->highGRange << 0; // FS_XL_HG
	writeSPI(LSM6DSV320_CTRL1_XL_HG, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL1_XL_HG, out);

	HAL_Delay(1);

	// CTRL2_XL_HG page 91, table 145
	config  = 0;
	config |= 0b0  << 4; // HG_USR_OFF_ON_WU doesn't drive the offset values into high-g wake-up and shock
	config |= 0b00 << 0; // XL_HG_ST sets the normal self-test mode (default)
	writeSPI(LSM6DSV320_CTRL2_XL_HG, &config);
	out[0] = 0; out[1] = 0;
	readSPI(LSM6DSV320_CTRL2_XL_HG, out);

	HAL_Delay(1);

	/* Configure Sensitivities */

	// Set the according Low G Sensitivity
	switch (lowGRange) {
		case LOW_G_RANGE_2G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_2G  * GRAVITY; break;
		case LOW_G_RANGE_4G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_4G  * GRAVITY; break;
		case LOW_G_RANGE_8G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_8G  * GRAVITY; break;
		case LOW_G_RANGE_16G: this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_16G * GRAVITY; break;
		default: return false;
	}

	// Set the according High G Sensitivity
	switch (highGRange) {
		case HIGH_G_RANGE_32G:  this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_32G  * GRAVITY; break;
		case HIGH_G_RANGE_64G:  this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_64G  * GRAVITY; break;
		case HIGH_G_RANGE_128G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_128G * GRAVITY; break;
		case HIGH_G_RANGE_256G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_256G * GRAVITY; break;
		case HIGH_G_RANGE_320G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_320G * GRAVITY; break;
		default: return false;
	}

	// Set the according Gyroscope Sensitivity
	switch (gyroRange) {
		case GYRO_RANGE_250DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_250DPS; break;
		case GYRO_RANGE_500DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_500DPS; break;
		case GYRO_RANGE_1000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_1000DPS; break;
		case GYRO_RANGE_2000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_2000DPS; break;
		case GYRO_RANGE_4000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_4000DPS; break;
		default: return false;
	}

	return 0;
}

int LSM6DSV320::updateDevice()
{
	// Read Gyroscope Measurements
	readSPI(LSM6DSV320_OUTX_L_G, &this->buffer[0]);
	readSPI(LSM6DSV320_OUTX_H_G, &this->buffer[1]);
	readSPI(LSM6DSV320_OUTY_L_G, &this->buffer[2]);
	readSPI(LSM6DSV320_OUTY_H_G, &this->buffer[3]);
	readSPI(LSM6DSV320_OUTZ_L_G, &this->buffer[4]);
	readSPI(LSM6DSV320_OUTZ_H_G, &this->buffer[5]);

	// Cast measurements to floats
	this->data->LSM6DSV320GyroZ = ((float) (int16_t) (this->buffer[0] | this->buffer[1] << 8)) * this->gyroSensitivity;
	this->data->LSM6DSV320GyroY = ((float) (int16_t) (this->buffer[2] | this->buffer[3] << 8)) * this->gyroSensitivity;
	this->data->LSM6DSV320GyroZ = ((float) (int16_t) (this->buffer[4] | this->buffer[5] << 8)) * this->gyroSensitivity;

	// Read Low G Accelerometer Measurements
	readSPI(LSM6DSV320_OUTX_L_A, &this->buffer[0]);
	readSPI(LSM6DSV320_OUTX_H_A, &this->buffer[1]);
	readSPI(LSM6DSV320_OUTY_L_A, &this->buffer[2]);
	readSPI(LSM6DSV320_OUTY_H_A, &this->buffer[3]);
	readSPI(LSM6DSV320_OUTZ_L_A, &this->buffer[4]);
	readSPI(LSM6DSV320_OUTZ_H_A, &this->buffer[5]);

	// Cast measurements to floats
	this->data->LSM6DSV320LowGAccelX = ((float) (int16_t) (this->buffer[0] | this->buffer[1] << 8)) * this->lowGSensitivity;
	this->data->LSM6DSV320LowGAccelZ = ((float) (int16_t) (this->buffer[2] | this->buffer[3] << 8)) * this->lowGSensitivity;
	this->data->LSM6DSV320LowGAccelZ = ((float) (int16_t) (this->buffer[4] | this->buffer[5] << 8)) * this->lowGSensitivity;

	// Read High G Accelerometer Measurements
	readSPI(LSM6DSV320_UI_OUTX_L_A_OIS_HG, &this->buffer[0]);
	readSPI(LSM6DSV320_UI_OUTX_H_A_OIS_HG, &this->buffer[1]);
	readSPI(LSM6DSV320_UI_OUTY_L_A_OIS_HG, &this->buffer[2]);
	readSPI(LSM6DSV320_UI_OUTY_H_A_OIS_HG, &this->buffer[3]);
	readSPI(LSM6DSV320_UI_OUTZ_L_A_OIS_HG, &this->buffer[4]);
	readSPI(LSM6DSV320_UI_OUTZ_H_A_OIS_HG, &this->buffer[5]);

	// Cast measurements to floats
	this->data->LSM6DSV320HighGAccelX = ((float) (int16_t) (this->buffer[0] | this->buffer[1] << 8)) * this->highGSensitivity;
	this->data->LSM6DSV320HighGAccelY = ((float) (int16_t) (this->buffer[2] | this->buffer[3] << 8)) * this->highGSensitivity;
	this->data->LSM6DSV320HighGAccelZ = ((float) (int16_t) (this->buffer[4] | this->buffer[5] << 8)) * this->highGSensitivity;

	return 0;
}

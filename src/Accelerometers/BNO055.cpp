#include "BNO055.h"

#include "ColorLED.hpp"

BNO055::BNO055(Data *data) : Sensor(BNO055_ADDR, data, &Wire) {
	this->accX = 0.0f;
	this->accY = 0.0f;
	this->accZ = 0.0f;

	this->gyroX = 0.0f;
	this->gyroY = 0.0f;
	this->gyroZ = 0.0f;

	this->magX = 0.0f;
	this->magY = 0.0f;
	this->magZ = 0.0f;

	this->K = 1.f;

	// Accelerometer
	accSettings = 0b00000000;
	switch (accFreq) { // Set the frequency
		case FREQ_7_81:   accSettings |= 0b00000000; break;
		case FREQ_15_63:  accSettings |= 0b00000100; break;
		case FREQ_31_25:  accSettings |= 0b00001000; break;
		case FREQ_62_5:   accSettings |= 0b00001100; break;
		case FREQ_125:    accSettings |= 0b00010000; break;
		case FREQ_250:    accSettings |= 0b00010100; break;
		case FREQ_500:    accSettings |= 0b00011000; break;
		case FREQ_1000:   accSettings |= 0b00011100; break;
	}

	switch (accRange) { // Set the range
		case RANGE_2:  accSettings |= 0b00000000; ACC_SCALE = 4.0 / 32768.0;  break;
		case RANGE_4:  accSettings |= 0b00000001; ACC_SCALE = 8.0 / 32768.0;  break;
		case RANGE_8:  accSettings |= 0b00000010; ACC_SCALE = 16.0 / 32768.0; break;
		case RANGE_16: accSettings |= 0b00000011; ACC_SCALE = 32.0 / 32768.0; break;
	}

	// Gyroscope
	gyroSettings = 0b00000010;
	switch (gyroFreq) { // Set the frequency
		case FREQ_523: gyroSettings |= 0b00000000; break;
		case FREQ_230: gyroSettings |= 0b00001000; break;
		case FREQ_116: gyroSettings |= 0b00010000; break;
		case FREQ_47:  gyroSettings |= 0b00011000; break;
		case FREQ_23:  gyroSettings |= 0b00100000; break;
		case FREQ_12:  gyroSettings |= 0b00101000; break;
		case FREQ_64:  gyroSettings |= 0b00110000; break;
		case FREQ_32:  gyroSettings |= 0b00111000; break;
	}

	switch (gyroRange) { // Set the range
		case RANGE_2000: gyroSettings |= 0b00000000; GYRO_SCALE = 4000.0 / 32768.0; break;
		case RANGE_1000: gyroSettings |= 0b00000001; GYRO_SCALE = 2000.0 / 32768.0; break;
		case RANGE_500:  gyroSettings |= 0b00000010; GYRO_SCALE = 1000.0 / 32768.0; break;
		case RANGE_250:  gyroSettings |= 0b00000011; GYRO_SCALE = 500.0 / 32768.0;  break;
		case RANGE_125:  gyroSettings |= 0b00000100; GYRO_SCALE = 256.0 / 32768.0;  break;
	}

	// Magnetometer
	magSettings = 0b00001000;
	switch (magFreq) { // Set the frequency
		case FREQ_2:  magSettings |= 0b00000000; break;
		case FREQ_6:  magSettings |= 0b00000001; break;
		case FREQ_8:  magSettings |= 0b00000010; break;
		case FREQ_10: magSettings |= 0b00000011; break;
		case FREQ_15: magSettings |= 0b00000100; break;
		case FREQ_20: magSettings |= 0b00000101; break;
		case FREQ_25: magSettings |= 0b00000110; break;
		case FREQ_30: magSettings |= 0b00000111; break;
	}

	MAG_SCALE = 8.0 / 32768.0;
}

bool BNO055::begin() {
	byte chip_id[1];
	read(CHIP_ID_REG, chip_id);

	if (chip_id[0] != 0xA0) {
		Serial.println("BNO055 Chip ID incorrect");
		Serial.print("Chip ID: ");
		Serial.println(chip_id[0], HEX);
		return false;
	}

	delay(100);

	// Put BN0055 into config mode
	write(OPR_MODE_REG, 0x00);
	delay(25);

	// Change to page 1
	write(PAGE_ID_REG, 0x01);

	// Set the Accelerometer config
	write(ACC_CONFIG_REG, accSettings);

	// Set the Gyro config 0
	write(GYR_CONFIG_0_REG, gyroSettings);

	// Set the Gyro config 1 to
	write(GYR_CONFIG_1_REG, 0b00000000);

	// Set the Magnetometer config to
	write(MAG_CONFIG_REG, magSettings);

	// Change to page 0
	write(PAGE_ID_REG, 0x00);

	// Set Power Mode to 0x00 Normal Mode
	write(PWR_MODE_REG, 0x00);

	// Set Units to, 10000000
	write(UNIT_SEL_REG, 0b10000000);

	// // Set the Axis Map to switch the X, and Z axis. Set to 00000110
	// write(AXIS_MAP_CONFIG, 0b00000110);

	// // Set the Axis Map Sign to flip all of the axis. Set to 00000111
	// write(AXIS_MAP_SIGN, 0b00000111);

	// Set the Operating mode to, NDOF 00001100: 0x0C
	write(OPR_MODE_REG, 0b00000111);

	delay(25);
	update_sensor();

	write_calibrations();

	ColorLED::show_red();
	Serial.println("Calibration the accelerometer");

	// Make sure we get a good calibration at least 5 times
	for (int i=0; i<5; i++) {
		// While not calibrated
		while (this->get_calibration_status(true) != 255) {
			i = 0; // Reset the counter if we don't have a good calibration
			delay(50);

			// Check if the button is pressed, if it is, cancel the calibration
			if (digitalRead(14) == HIGH) {
				Serial.println("Calibration canceled");
				i = 5;
				break;
			}
		}
	}

	Serial.println("Calibration finished\n");

	delay(20);

	// Put BN0055 into config mode
	write(OPR_MODE_REG, 0x00);
	delay(25);

	// Set the Operating mode to, AMG 00000111
	write(OPR_MODE_REG, 0b00001100);
	delay(20);

	return true;
}

void BNO055::get_data() {
	if ((millis() - lastUpdate) >= updateInt) {
		update_sensor();

		data->acc.X = accX;
		data->acc.Y = accY;
		data->acc.Z = accZ;

		data->gyro.X = gyroX;
		data->gyro.Y = gyroY;
		data->gyro.Z = gyroZ;

		data->mag.X = magX;
		data->mag.Y = magY;
		data->mag.Z = magZ;

		data->newImuData = true;

		// https://stackoverflow.com/questions/78999123/how-can-i-estimate-an-orientation-just-using-a-gyro-and-magnetometer

		data->orientation.X = K * (data->orientation.X + gyroX); // Pitch
		data->orientation.Y = K * (data->orientation.Y + gyroY); // Roll
		data->orientation.Z = K * (data->orientation.Z + gyroZ); // Yaw

		lastUpdate = millis();
	} else {
		data->newImuData = false;
	}
}

void BNO055::update_sensor() {
	byte raw_data[6];

	// Check if it is time to read the accelerometer
	if ((millis() - accLast) >= accInterval) {
		// The accelerometer data is stored in the first 6 bytes
		read(ACC_DATA_X_LSB_REG, raw_data, 6);

		this->accX = (static_cast<float>((int16_t)(raw_data[0] | (raw_data[1] << 8))) / 100.0) - accXoffset;
		this->accY = (static_cast<float>((int16_t)(raw_data[2] | (raw_data[3] << 8))) / 100.0) - accYoffset;
		this->accZ = (static_cast<float>((int16_t)(raw_data[4] | (raw_data[5] << 8))) / 100.0) - accZoffset;

		accLast = millis();
	}

	// Check if it is time to read the gyroscope
	if ((millis() - gyroLast) >= gyroInterval) {
		// The gyroscope data is stored in the next 6 bytes
		read(GYR_DATA_X_LSB_REG, raw_data, 6);

		this->gyroX = ((float) ((int16_t)(raw_data[0] | ((int16_t)raw_data[1] << 8))) / 900.0) - gyrXoffset;
		this->gyroY = ((float) ((int16_t)(raw_data[2] | ((int16_t)raw_data[3] << 8))) / 900.0) - gyrYoffset;
		this->gyroZ = ((float) ((int16_t)(raw_data[4] | ((int16_t)raw_data[5] << 8))) / 900.0) - gyrZoffset;

		gyroLast = millis();
	}

	// Check if it is time to read the magnetometer
	if ((millis() - magLast) >= magInterval) {
		// The magnetometer data is stored in the next 6 bytes
		read(MAG_DATA_X_LSB_REG, raw_data, 6);

		this->magX = (float) ((int16_t)(raw_data[0] | ((int16_t)raw_data[1] << 8)) / 1.6);
		this->magY = (float) ((int16_t)(raw_data[2] | ((int16_t)raw_data[3] << 8)) / 1.6);
		this->magZ = (float) ((int16_t)(raw_data[4] | ((int16_t)raw_data[5] << 8)) / 1.6);

		magLast = millis();
	}
}

void BNO055::write_calibrations() {
	// Put BN0055 into config mode
	write(OPR_MODE_REG, 0x00);
	delay(25);

	// Write the accelerometer calibration
	write(ACC_OFFSET_X_LSB_REG,    accXcal & 0xFF);
	write(ACC_OFFSET_X_LSB_REG+1, (accXcal >> 8) & 0xFF);

	write(ACC_OFFSET_X_LSB_REG+2,  accYcal & 0xFF);
	write(ACC_OFFSET_X_LSB_REG+3, (accYcal >> 8) & 0xFF);

	write(ACC_OFFSET_X_LSB_REG+4,  accZcal & 0xFF);
	write(ACC_OFFSET_X_LSB_REG+5, (accZcal >> 8) & 0xFF);

	// Write the gyroscope calibration
	write(GYR_OFFSET_X_LSB_REG,    gyrXcal & 0xFF);
	write(GYR_OFFSET_X_LSB_REG+1, (gyrXcal >> 8) & 0xFF);

	write(GYR_OFFSET_X_LSB_REG+2,  gyrYcal & 0xFF);
	write(GYR_OFFSET_X_LSB_REG+3, (gyrYcal >> 8) & 0xFF);

	write(GYR_OFFSET_X_LSB_REG+4,  gyrZcal & 0xFF);
	write(GYR_OFFSET_X_LSB_REG+5, (gyrZcal >> 8) & 0xFF);

	// Write the magnetometer calibration
	write(MAG_OFFSET_X_LSB_REG,    magXcal & 0xFF);
	write(MAG_OFFSET_X_LSB_REG+1, (magXcal >> 8) & 0xFF);

	write(MAG_OFFSET_X_LSB_REG+2,  magYcal & 0xFF);
	write(MAG_OFFSET_X_LSB_REG+3, (magYcal >> 8) & 0xFF);

	write(MAG_OFFSET_X_LSB_REG+4,  magZcal & 0xFF);
	write(MAG_OFFSET_X_LSB_REG+5, (magZcal >> 8) & 0xFF);

	// Put BN0055 last mode
	write(OPR_MODE_REG, 0b00001100);
	delay(25);
}

void BNO055::reset() {

}

int BNO055::get_calibration_status(bool print) {
	byte calib_status[1];
	read(CALIB_STAT_REG, calib_status);

	if (print) {
		uint8_t system  = (calib_status[0] >> 6) & 0x03;  // Bits 7-6 represent Sys calibration
		uint8_t gyro    = (calib_status[0] >> 4) & 0x03;  // Bits 5-4 represent Gyro calibration
		uint8_t accel   = (calib_status[0] >> 2) & 0x03;  // Bits 3-2 represent Accel calibration
		uint8_t mag     = calib_status[0] & 0x03;         // Bits 1-0 represent Mag calibration

		if (system != 3) {
			Serial.print("System: ");
			Serial.print(system, BIN);
			Serial.print(", ");
		}

		if (gyro != 3) {
			Serial.print("Gyro: ");
			Serial.print(gyro, BIN);
			Serial.print(", ");
		}

		if (accel != 3) {
			Serial.print("Accel: ");
			Serial.print(accel, BIN);
			Serial.print(", ");
		}

		if (mag != 3) {
			Serial.print("Mag: ");
			Serial.print(mag, BIN);
		}

		if (calib_status[0] != 255)
			Serial.println(" Are not Fully calibrated!");
	}

	return calib_status[0];
}

void BNO055::get_calibration() {
	Serial.println("Fully calibrated!");
	Serial.print("Calibration: ");

	byte calib[18];
	read(ACC_OFFSET_X_LSB_REG, calib, 18);

	for (int i=0; i<18; i+=2) {
		// combine the two bytes into one 16 bit number
		int16_t offset = (int16_t)(calib[i] | ((int16_t)calib[i+1] << 8));
		Serial.print(offset);
		Serial.print(", ");
	}

	Serial.println();
}
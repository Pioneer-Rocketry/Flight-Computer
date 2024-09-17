// Accelerometers/BNO055.h
#ifndef BNO055_h
#define BNO055_h

#include <Arduino.h>
#include <Wire.h>

#include "Memory.h"
#include "Data.hpp"
#include "Sensor.hpp"

#define BNO055_ADDR 0x29

class BNO055 : Sensor {
	private:
		// Registers
		const int CHIP_ID_REG     = 0x00;
		const int OPR_MODE_REG    = 0x3D;
		const int PAGE_ID_REG     = 0x07;
		const int CALIB_STAT_REG  = 0x35;

		// Config Registers
		const int ACC_CONFIG_REG    = 0x08;
		const int GYR_CONFIG_0_REG  = 0x0A;
		const int GYR_CONFIG_1_REG  = 0x0B;
		const int MAG_CONFIG_REG    = 0x09;
		const int PWR_MODE_REG      = 0x3E;
		const int UNIT_SEL_REG      = 0x3B;
		const int AXIS_MAP_CONFIG   = 0x41;
		const int AXIS_MAP_SIGN     = 0x42;

		// Data Registers
		const int GRV_DATA_X_LSB_REG  = 0x2F;
		const int LIA_DATA_X_LSB_REG  = 0x28;
		const int QUA_DATA_X_LSB_REG  = 0x20;
		const int EUL_Heading_LSB_REG = 0x1A;
		const int MAG_DATA_X_LSB_REG  = 0x0E;
		const int GYR_DATA_X_LSB_REG  = 0x14;
		const int ACC_DATA_X_LSB_REG  = 0x08;

		// Offsets Registers
		const int ACC_OFFSET_X_LSB_REG = 0x55;
		const int MAG_OFFSET_X_LSB_REG = 0x5B;
		const int GYR_OFFSET_X_LSB_REG = 0x61;

		// Calibration offset
		const int16_t accXcal = 29;
		const int16_t accYcal = -8;
		const int16_t accZcal = 0;

		const int16_t magXcal = 33;
		const int16_t magYcal = 18;
		const int16_t magZcal = -89;

		const int16_t gyrXcal = -1;
		const int16_t gyrYcal = 0;
		const int16_t gyrZcal = 2;

		// Bias
		const float accXoffset = 0;
		const float accYoffset = 0;
		const float accZoffset = 0;

		const float magXoffset = 0;
		const float macYoffset = 0;
		const float magZoffset = 0;

		const float gyrXoffset = 0;
		const float gyrYoffset = 0;
		const float gyrZoffset = 0;

		// Data
		float accX,  accY,  accZ;
		float gyroX, gyroY, gyroZ;
		float magX,  magY,  magZ;

		int updateFreq = 30; // 30 hz
		float updateInt = 1000 / updateFreq;
		float lastUpdate;

		// Settings
		// Accelerometer
		enum AccelFreq { FREQ_7_81, FREQ_15_63, FREQ_31_25, FREQ_62_5, FREQ_125, FREQ_250, FREQ_500, FREQ_1000 };
		enum AccelRange { RANGE_2, RANGE_4, RANGE_8, RANGE_16 };

		AccelFreq accFreq = FREQ_31_25; // the frequency of the accelerometer in hz   options: 7.81, 15.63, 31.25, 62.5, 125, 250, 500, 1000
		AccelRange accRange = RANGE_4;  // the range of the accelerometer in g        options: 2, 4, 8, 16

		int accSettings = 0b00000000; // the settings for the accelerometer
		float ACC_SCALE;  // Scaling factor for accelerometer

		// Gyroscope
		enum GyroFreq { FREQ_523, FREQ_230, FREQ_116, FREQ_47, FREQ_23, FREQ_12, FREQ_64, FREQ_32 };
		enum GyroRange { RANGE_2000, RANGE_1000, RANGE_500, RANGE_250, RANGE_125 };

		GyroFreq gyroFreq = FREQ_32;      // the frequency of the gyro in hz            options: 523, 230, 116, 47, 23, 12, 64, 32
		GyroRange gyroRange = RANGE_2000; // the range of the gyro in dps               options: 2000, 1000, 500, 250, 125

		int gyroSettings = 0b00000000; // the settings for the gyro
		float GYRO_SCALE; // Scaling factor for gyroscope

		// Magnetometer
		enum MagFreq { FREQ_2, FREQ_6, FREQ_8, FREQ_10, FREQ_15, FREQ_20, FREQ_25, FREQ_30 };
		MagFreq magFreq = FREQ_30; // the frequency of the magnetometer in hz    options: 2, 6, 8, 10, 15, 20, 25, 30

		int magSettings = 0b00000000; // the settings for the magnetometer
		float MAG_SCALE; // Scaling factor for magnetometer (±4 µT)

		// Timings
		unsigned long accInterval = accFreq / 1000.0;
		unsigned long accLast = 0.0;

		unsigned long gyroInterval = gyroFreq / 1000.0;
		unsigned long gyroLast = 0.0;

		unsigned long magInterval = magFreq / 1000.0;
		unsigned long magLast = 0.0;

		/* Functions */
		void update_sensor() override;

	public:
		void get_data() override;
		bool begin() override;
		void reset();

		int get_calibration_status(bool print);
		void get_calibration();
		void write_calibrations();

		BNO055(Data *data);

};

/** Notes
 * Operating Mode: AMG Non-Fusion Mode (ACC, MAG, GYRO)
 * - Write 0x07 to register 0x3D OPR_MODE
 * - Why not use Sensor Fusion? Cause it can only handle up to +-4G we should use +-16G
 *   - Found Here https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BNO055-Range-in-Fusion-mode/td-p/13437
 * 
 * Acc Radius is the same as Acc Range?
 * 
 * Power Modes:
 * - Leave in normal
 * 
 * We could remap so the Z is up, and X is forward
 * - send 00000110 to register 0x41 - BNO055_AXIS_MAP_CONFIG
 * - If we need to flip any axis, we can send 00000[0,1][0,1][0,1] to register 0x42 - BNO055_AXIS_MAP_SIGN
 * 
 * Config -- Register Map Page 1
 * - Accelerometer: Range: 16G,     Bandwidth: 31.25Hz, Units: m/s^2, ACC_Config  0x0B
 * - Gyroscope:     Range: 500dps,  Bandwidth: 47Hz,    Units: dps,   GYR_Config_1 0x1A, GYR_Config_0 0x00
 * - Magnetometer:                  Data Rate: 30Hz,    Units: uT,    MAG_Config  0xFF
 * - Units:         Acceleration: m/s^2, Gyroscope: dps, Magnetometer: uT : UNIT_SEL 0x80
 * 
 * Output registers -- Register Map Page 0
 * - Accelerometer:  Accel_Data_X/Y/Z, Signed, 2 Bytes Each
 * - Gyroscope:      Gyro_Data_X/Y/Z,  Signed, 2 Bytes Each
 * - Magnetometer:   Mag_Data_X/Y/Z,   Signed, 2 Bytes Each
 * 
 * Switch Pages by settings PAGE_ID (0x07) to 0x01 for Page 1, and 0x00 for Page 0
 * 
 * Calibration
 * - Accelerometer: Place the sensor in 6 different stable positions for a few seconds each, slowly moving between positions
 * - Gyroscope:     Place the sensor in 1 stable position for a few seconds
 * - Magnetometer:   
 *   Check CALIB_STAT register (0x35) for calibration status
 *   We can save calibration profiles?
*/

#endif
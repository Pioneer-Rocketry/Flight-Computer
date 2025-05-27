#include "sensors/imu_LSM6DSV320.h"

IMU_LSM6DSV320::IMU_LSM6DSV320(I2C_HandleTypeDef *i2cHandler, Data *data)
    : Sensor(i2cHandler, LSM6DSV320_ADDRESS, data) {
}

bool IMU_LSM6DSV320::begin() {
    uint8_t whoAmI;

    read_I2C(LSM6DSV320_WHO_AM_I, &whoAmI);
    if (whoAmI != LSM6DSV320_ID) {
        return false;
    }

    /* Sensor Configuration */
    uint8_t config = 0;

    // CTRL1 page 65, table 52
    config  = 0;
    config |= this->lowGOPMode << 4; // OP_MODE_XL
    config |= this->lowGODR    << 0; // ODR_XL
    write_I2C(LSM6DSV320_CTRL1, &config);

    // CTRL2 page 66, table 55
    config  = 0;
    config |= this->gyroOPMode << 4; // OP_MODE_G
    config |= this->gyroODR    << 0; // ODR_G
    write_I2C(LSM6DSV320_CTRL2, &config);

    // CTRL3 page 67, table 58
    config  = 0;
    config |= 0b0 << 7; // BOOT set to normal mode
    config |= 0b1 << 6; // BDU set to
    config |= 0b1 << 2; // IF_INC set to
    config |= 0b0 << 0; // SW_RESET set to normal mode
    write_I2C(LSM6DSV320_CTRL3, &config);

    // CTRL4 page 68, table 60
    config  = 0;
    config |= 0b0 << 4; // INT2_on_INT1
    config |= 0b0 << 3; // DRDY_MASK set to disabled
    config |= 0b0 << 2; // INT2_DRDY_TEMP set to disabled
    config |= 0b0 << 1; // DRDY_PULSED set data-ready latched mode
    config |= 0b0 << 0; // INT2_IN_LH set embedded functions active low
    write_I2C(LSM6DSV320_CTRL4, &config);

    // CTRL5 page 69, table 62
    config  = 0;
    config |= 0b0  << 7; // IF2_TA0_PID
    config |= 0b00 << 2; // BUS_ACT_SEC sets the bus available time selection for IBT to 50 us (default)
    config |= 0b0  << 0; // INT_EN_I2C disables INT pin when I3C is enabled (default)
    write_I2C(LSM6DSV320_CTRL5, &config);

    // CTRL6 page 69, table 64
    config  = 0;
    config |= 0b000           << 4; // LPF1_G_BW
    config |= this->gyroRange << 0; // FS_G
    write_I2C(LSM6DSV320_CTRL6, &config);

    // CTRL7 page 70, table 67
    config  = 0;
    config |= 0b0 << 7; // INT1_DRDY_XL disabled high-g data-ready interrupt on INT1 pin (default)
    config |= 0b0 << 6; // INT2_DRDY_XL disables high-g data-ready interrupt on INT2 pin (default)
    config |= 0b0 << 0; // LPF1_G_EN disables low-pass filter for gyroscope
    write_I2C(LSM6DSV320_CTRL7, &config);

    // CTRL8 page 71, table 69
    config  = 0;
    config |= 0b000           << 5; // HP_LPF2_XL_BW
    config |= this->lowGRange << 0; // FS_XL
    write_I2C(LSM6DSV320_CTRL8, &config);

    // CTRL9 page 72, table 72
    config  = 0;
    config |= 0b0 << 6; // HP_REF_MODE_XL disables the high-pass filter reference mode (default)
    config |= 0b0 << 5; // XL_FASTSETTL_MODE disables the fast-settling mode (default)
    config |= 0b0 << 4; // HP_SLOPE_XL_EN selects low-pass filter slope (default)
    config |= 0b0 << 3; // LPF2_XL_EN uses first stage of digital filtering (default)
    config |= 0b0 << 1; // USR_OFF_W sets user offsets to be 2^-10 g/LSB (default)
    config |= 0b0 << 0; // USR_OFF_ON_OUT bypasses user offset correction block (default)
    write_I2C(LSM6DSV320_CTRL9, &config);

    // CTRL10 page 74, table 74
    config  = 0;
    config |= 0b0  << 6; // EMB_FUNC_DEBUG disables the embedded functions
    config |= 0b00 << 2; // ST_G sets the normal self-test mode (default)
    config |= 0b00 << 0; // ST_XL sets the normal self-test mode (default)
    write_I2C(LSM6DSV320_CTRL10, &config);

    // CTRL1_XL_HG page 91, table 147
    config  = 0;
    config |= 0b1              << 7; // XL_HG_REGOUT_EN enable high-g outputs
    config |= 0b0              << 6; // HG_USR_OUFF_OU_OUT disabled high-g offsets
    config |= this->highGODR   << 3; // ODR_XL_HG
    config |= this->highGRange << 0; // FS_XL_HG
    write_I2C(LSM6DSV320_CTRL1_XL_HG, &config);

    // CTRL2_XL_HG page 91, table 145
    config  = 0;
    config |= 0b0  << 4; // HG_USR_OFF_ON_WU doesnt drive the offset values into high-g wake-up and shock
    config |= 0b00 << 0; // XL_HG_ST sets the normal self-test mode (default)
    write_I2C(LSM6DSV320_CTRL2_XL_HG, &config);


    /* Configure Sensitivities */

    // Set the according Low G Sensitivity
    switch (lowGRange) {
        case LOW_G_RANGE_2G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_2G;  break;
        case LOW_G_RANGE_4G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_4G;  break;
        case LOW_G_RANGE_8G:  this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_8G;  break;
        case LOW_G_RANGE_16G: this->lowGSensitivity = LSM6DSV320_LOW_G_SENSITIVITY_16G; break;
        default: return false;
    }

    // Set the according High G Sensitivity
    switch (highGRange) {
        case HIGH_G_RANGE_32G:  this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_32G;  break;
        case HIGH_G_RANGE_64G:  this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_64G;  break;
        case HIGH_G_RANGE_128G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_128G; break;
        case HIGH_G_RANGE_256G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_256G; break;
        case HIGH_G_RANGE_320G: this->highGSensitivity = LSM6DSV320_HIGH_G_SENSITIVITY_320G; break;
        default: return false;
    }

    // Set the according Gyro Sensitivity
    switch (gyroRange) {
        case GYRO_RANGE_250DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_250DPS;  break;
        case GYRO_RANGE_500DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_500DPS;  break;
        case GYRO_RANGE_1000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_1000DPS; break;
        case GYRO_RANGE_2000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_2000DPS; break;
        case GYRO_RANGE_4000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_4000DPS; break;
        default: return false;
    }

    return true;
}

void IMU_LSM6DSV320::get_data() {
    uint8_t buffer[6];

    // Read Gyroscope Measurements
    read_I2C(LSM6DSV320_OUTX_L_G, &buffer[0], 2);
    read_I2C(LSM6DSV320_OUTY_L_G, &buffer[2], 2);
    read_I2C(LSM6DSV320_OUTZ_L_G, &buffer[4], 2);

    // Cast measurements to floats
    this->data->LSM6DSV320_Gyro.setX(((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->gyroSensitivity);
    this->data->LSM6DSV320_Gyro.setY(((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->gyroSensitivity);
    this->data->LSM6DSV320_Gyro.setZ(((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->gyroSensitivity);

    // Read Low G Accelerometer Measurements
    read_I2C(LSM6DSV320_OUTX_L_A, &buffer[0], 2);
    read_I2C(LSM6DSV320_OUTY_L_A, &buffer[2], 2);
    read_I2C(LSM6DSV320_OUTZ_L_A, &buffer[4], 2);

    // Cast measurements to floats
    this->data->LSM6DSV320_LowG_Accel.setX(((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->lowGSensitivity);
    this->data->LSM6DSV320_LowG_Accel.setY(((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->lowGSensitivity);
    this->data->LSM6DSV320_LowG_Accel.setZ(((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->lowGSensitivity);

    // Read High G Accelerometer Measurements
    read_I2C(LSM6DSV320_UI_OUTX_L_A_OIS_HG, &buffer[0], 2);
    read_I2C(LSM6DSV320_UI_OUTY_L_A_OIS_HG, &buffer[2], 2);
    read_I2C(LSM6DSV320_UI_OUTZ_L_A_OIS_HG, &buffer[4], 2);

    // Cast measurements to floats
    this->data->LSM6DSV320_HighG_Accel.setX(((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->highGSensitivity);
    this->data->LSM6DSV320_HighG_Accel.setY(((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->highGSensitivity);
    this->data->LSM6DSV320_HighG_Accel.setZ(((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->highGSensitivity);

}
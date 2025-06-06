#include "SPI_Devices/imu_LSM6DSV320.h"

IMU_LSM6DSV320::IMU_LSM6DSV320(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPI_Device(data, spi, port, pin, "LSM6DSV320") {
}

bool IMU_LSM6DSV320::begin() {
    uint8_t whoAmI = 0;

    read_SPI(LSM6DSV320_WHO_AM_I, &whoAmI);
    if (whoAmI != LSM6DSV320_ID) {
        return false;
    }

    /* Sensor Configuration */
    uint8_t config = 0;
    uint8_t out[2] = {0,0};

    // IF_CFG page 58, table 32
    config  = 0;
    config |= 0 << 7; // SDA_PU_EN disconnects the SDA pullup (default)
    config |= 0 << 6; // SHUB_PU_EN Disables I2C plus pullups (default)
    config |= 0 << 5; // ASF_CTRL turns off antispike filter (default)
    config |= 0 << 4; // H_LACTIVE sets interrupts to active high (default)
    config |= 0 << 3; // PP_OD sets interrupts to push-pull (default)
    config |= 0 << 2; // SIM sets the SPI interface more to 4 wire (default)
    config |= 0 << 1; // Reserved
    config |= 0 << 0; // I2C_I3C_DISABLE disables I2C and I3C interface (default)
    write_SPI(LSM6DSV320_IF_CFG, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_IF_CFG, out);

    HAL_Delay(1);

    // CTRL1 page 65, table 52
    config  = 0;
    config |= this->lowGOPMode << 4; // OP_MODE_XL
    config |= this->lowGODR    << 0; // ODR_XL
    write_SPI(LSM6DSV320_CTRL1, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL1, out);

    HAL_Delay(1);

    // CTRL2 page 66, table 55
    config  = 0;
    config |= this->gyroOPMode << 4; // OP_MODE_G
    config |= this->gyroODR    << 0; // ODR_G
    write_SPI(LSM6DSV320_CTRL2, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL2, out);

    HAL_Delay(1);

    // CTRL3 page 67, table 58
    config  = 0;
    config |= 0b0 << 7; // BOOT set to normal mode
    config |= 0b1 << 6; // BDU set to
    config |= 0b1 << 2; // IF_INC set to
    config |= 0b0 << 0; // SW_RESET set to normal mode
    write_SPI(LSM6DSV320_CTRL3, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL3, out);

    HAL_Delay(1);

    // CTRL4 page 68, table 60
    config  = 0;
    config |= 0b0 << 4; // INT2_on_INT1
    config |= 0b0 << 3; // DRDY_MASK set to disabled
    config |= 0b0 << 2; // INT2_DRDY_TEMP set to disabled
    config |= 0b0 << 1; // DRDY_PULSED set data-ready latched mode
    config |= 0b0 << 0; // INT2_IN_LH set embedded functions active low
    write_SPI(LSM6DSV320_CTRL4, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL4, out);

    HAL_Delay(1);

    // CTRL5 page 69, table 62
    config  = 0;
    config |= 0b0  << 7; // IF2_TA0_PID
    config |= 0b00 << 2; // BUS_ACT_SEC sets the bus available time selection for IBT to 50 us (default)
    config |= 0b0  << 0; // INT_EN_SPI disables INT pin when I3C is enabled (default)
    write_SPI(LSM6DSV320_CTRL5, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL5, out);

    HAL_Delay(1);

    // CTRL6 page 69, table 64
    config  = 0;
    config |= 0b110           << 4; // LPF1_G_BW sets the low-pass filter for gyroscope bandwidth selection
    config |= this->gyroRange << 0; // FS_G
    write_SPI(LSM6DSV320_CTRL6, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL6, out);

    HAL_Delay(1);

    // CTRL7 page 70, table 67
    config  = 0;
    config |= 0b0 << 7; // INT1_DRDY_XL disabled high-g data-ready interrupt on INT1 pin (default)
    config |= 0b0 << 6; // INT2_DRDY_XL disables high-g data-ready interrupt on INT2 pin (default)
    config |= 0b1 << 0; // LPF1_G_EN enables low-pass filter for gyroscope (default)
    write_SPI(LSM6DSV320_CTRL7, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL7, out);

    HAL_Delay(1);

    // CTRL8 page 71, table 69
    config  = 0;
    config |= 0b000           << 5; // HP_LPF2_XL_BW set to ODR / 4
    config |= this->lowGRange << 0; // FS_XL
    write_SPI(LSM6DSV320_CTRL8, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL8, out);

    HAL_Delay(1);

    // CTRL9 page 72, table 72
    config  = 0;
    config |= 0b0 << 6; // HP_REF_MODE_XL disables the high-pass filter reference mode (default)
    config |= 0b0 << 5; // XL_FASTSETTL_MODE disables the fast-settling mode (default)
    config |= 0b0 << 4; // HP_SLOPE_XL_EN selects low-pass filter slope (default)
    config |= 0b1 << 3; // LPF2_XL_EN 
    config |= 0b0 << 1; // USR_OFF_W sets user offsets to be 2^-10 g/LSB (default)
    config |= 0b0 << 0; // USR_OFF_ON_OUT bypasses user offset correction block (default)
    write_SPI(LSM6DSV320_CTRL9, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL9, out);

    HAL_Delay(1);

    // CTRL10 page 74, table 74
    config  = 0;
    config |= 0b0  << 6; // EMB_FUNC_DEBUG disables the embedded functions
    config |= 0b00 << 2; // ST_G sets the normal self-test mode (default)
    config |= 0b00 << 0; // ST_XL sets the normal self-test mode (default)
    write_SPI(LSM6DSV320_CTRL10, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL10, out);

    // CTRL1_XL_HG page 91, table 147
    config  = 0;
    config |= 0b1              << 7; // XL_HG_REGOUT_EN enable high-g outputs
    config |= 0b0              << 6; // HG_USR_OUFF_OU_OUT disabled high-g offsets
    config |= this->highGODR   << 3; // ODR_XL_HG
    config |= this->highGRange << 0; // FS_XL_HG
    write_SPI(LSM6DSV320_CTRL1_XL_HG, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL1_XL_HG, out);

    HAL_Delay(1);

    // CTRL2_XL_HG page 91, table 145
    config  = 0;
    config |= 0b0  << 4; // HG_USR_OFF_ON_WU doesnt drive the offset values into high-g wake-up and shock
    config |= 0b00 << 0; // XL_HG_ST sets the normal self-test mode (default)
    write_SPI(LSM6DSV320_CTRL2_XL_HG, &config);
    out[0] = 0; out[1] = 0;
    read_SPI(LSM6DSV320_CTRL2_XL_HG, out);

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

    // Set the according Gyro Sensitivity
    switch (gyroRange) {
        case GYRO_RANGE_250DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_250DPS; break;
        case GYRO_RANGE_500DPS:  this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_500DPS; break;
        case GYRO_RANGE_1000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_1000DPS; break;
        case GYRO_RANGE_2000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_2000DPS; break;
        case GYRO_RANGE_4000DPS: this->gyroSensitivity = LSM6DSV320_GYRO_SENSITIVITY_4000DPS; break;
        default: return false;
    }

    HAL_Delay(2000); // Wait 2 seconds to make sure the sensor is stable

    // Read 100 samples of the Gyroscope and Accelerometer
    int calibrationSamples = 100;
    for (int i = 0; i < calibrationSamples; i++) {
        uint8_t buffer[6];

        // Read Gyroscope Measurements
        read_SPI(LSM6DSV320_OUTX_L_G, &buffer[0]);
        read_SPI(LSM6DSV320_OUTX_H_G, &buffer[1]);
        read_SPI(LSM6DSV320_OUTY_L_G, &buffer[2]);
        read_SPI(LSM6DSV320_OUTY_H_G, &buffer[3]);
        read_SPI(LSM6DSV320_OUTZ_L_G, &buffer[4]);
        read_SPI(LSM6DSV320_OUTZ_H_G, &buffer[5]);

        gyroOffsetX += ((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->gyroSensitivity;
        gyroOffsetY += ((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->gyroSensitivity;
        gyroOffsetZ += ((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->gyroSensitivity;

        HAL_Delay(10);
    }


    gyroOffsetX /= calibrationSamples;
    gyroOffsetY /= calibrationSamples;
    gyroOffsetZ /= calibrationSamples;

    return true;
}

void IMU_LSM6DSV320::loop() {
    uint8_t buffer[6];

    // Read Gyroscope Measurements
    read_SPI(LSM6DSV320_OUTX_L_G, &buffer[0]);
    read_SPI(LSM6DSV320_OUTX_H_G, &buffer[1]);
    read_SPI(LSM6DSV320_OUTY_L_G, &buffer[2]);
    read_SPI(LSM6DSV320_OUTY_H_G, &buffer[3]);
    read_SPI(LSM6DSV320_OUTZ_L_G, &buffer[4]);
    read_SPI(LSM6DSV320_OUTZ_H_G, &buffer[5]);

    // Cast measurements to floats
    this->data->LSM6DSV320_Gyro.setX((((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->gyroSensitivity) - gyroOffsetX);
    this->data->LSM6DSV320_Gyro.setY((((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->gyroSensitivity) - gyroOffsetY);
    this->data->LSM6DSV320_Gyro.setZ((((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->gyroSensitivity) - gyroOffsetZ);

    // Read Low G Accelerometer Measurements
    read_SPI(LSM6DSV320_OUTX_L_A, &buffer[0]);
    read_SPI(LSM6DSV320_OUTX_H_A, &buffer[1]);
    read_SPI(LSM6DSV320_OUTY_L_A, &buffer[2]);
    read_SPI(LSM6DSV320_OUTY_H_A, &buffer[3]);
    read_SPI(LSM6DSV320_OUTZ_L_A, &buffer[4]);
    read_SPI(LSM6DSV320_OUTZ_H_A, &buffer[5]);

    // Cast measurements to floats
    this->data->LSM6DSV320_LowG_Accel.setX(((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->lowGSensitivity);
    this->data->LSM6DSV320_LowG_Accel.setY(((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->lowGSensitivity);
    this->data->LSM6DSV320_LowG_Accel.setZ(((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->lowGSensitivity);

    // Read High G Accelerometer Measurements
    read_SPI(LSM6DSV320_UI_OUTX_L_A_OIS_HG, &buffer[0]);
    read_SPI(LSM6DSV320_UI_OUTX_H_A_OIS_HG, &buffer[1]);
    read_SPI(LSM6DSV320_UI_OUTY_L_A_OIS_HG, &buffer[2]);
    read_SPI(LSM6DSV320_UI_OUTY_H_A_OIS_HG, &buffer[3]);
    read_SPI(LSM6DSV320_UI_OUTZ_L_A_OIS_HG, &buffer[4]);
    read_SPI(LSM6DSV320_UI_OUTZ_H_A_OIS_HG, &buffer[5]);

    // Cast measurements to floats
    this->data->LSM6DSV320_HighG_Accel.setX(((float) (int16_t) (buffer[0] | buffer[1] << 8)) * this->highGSensitivity);
    this->data->LSM6DSV320_HighG_Accel.setY(((float) (int16_t) (buffer[2] | buffer[3] << 8)) * this->highGSensitivity);
    this->data->LSM6DSV320_HighG_Accel.setZ(((float) (int16_t) (buffer[4] | buffer[5] << 8)) * this->highGSensitivity);

}
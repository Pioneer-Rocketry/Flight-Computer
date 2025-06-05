#include "I2C_Devices/mag_MMC5603NJ.h"

Mag_MMC5603NJ::Mag_MMC5603NJ(I2C_HandleTypeDef *i2cHandler, Data *data)
    : I2C_Device(i2cHandler, MMC5603NJ_ADDRESS, data, "MMC5603NJ") {
}

bool Mag_MMC5603NJ::begin() {
    uint8_t whoAmI;

    read_I2C(MMC5603NJ_PRODUCT_ID, &whoAmI);
    if (whoAmI != MMC5603NJ_ID) {
        return false;
    }

    /* Sensor Configuration */
    uint8_t config = 0;

    // TODO: Implement sensor configuration

    return true;
}

void Mag_MMC5603NJ::loop() {
    uint8_t buffer[3];

    // Read X Axis Measurements
    read_I2C(MMC5603NJ_X_OUT_0, &buffer[0], 2);
    read_I2C(MMC5603NJ_X_OUT_2, &buffer[2], 1);

    this->data->MMC5603_Mag.setX(((float) (int32_t) (buffer[0] << 16 | buffer[1] << 8 | buffer[2] >> 4)) * MMC5603NJ_SENSITIVITY);

    // Read Y Axis Measurements
    read_I2C(MMC5603NJ_Y_OUT_0, &buffer[0], 2);
    read_I2C(MMC5603NJ_Y_OUT_2, &buffer[2], 1);

    this->data->MMC5603_Mag.setY(((float) (int32_t) (buffer[0] << 16 | buffer[1] << 8 | buffer[2] >> 4)) * MMC5603NJ_SENSITIVITY);

    // Read Z Axis Measurements
    read_I2C(MMC5603NJ_Z_OUT_0, &buffer[0], 2);
    read_I2C(MMC5603NJ_Z_OUT_2, &buffer[2], 1);

    this->data->MMC5603_Mag.setZ(((float) (int32_t) (buffer[0] << 16 | buffer[1] << 8 | buffer[2] >> 4)) * MMC5603NJ_SENSITIVITY);

}

#ifndef MAG_MMC5603NJ_H
#define MAG_MMC5603NJ_H

#include "sensors/sensors.hpp"

// Address
#define MMC5603NJ_ADDRESS 0b1101010 // or 0b1101011 if

// Register Addresses
#define MMC5603NJ_X_OUT_0               0x00
#define MMC5603NJ_X_OUT_1               0x01
#define MMC5603NJ_Y_OUT_0               0x02
#define MMC5603NJ_Y_OUT_1               0x03
#define MMC5603NJ_Z_OUT_0               0x04
#define MMC5603NJ_Z_OUT_1               0x05
#define MMC5603NJ_X_OUT_2               0x06
#define MMC5603NJ_Y_OUT_2               0x07
#define MMC5603NJ_Z_OUT_2               0x08
#define MMC5603NJ_T_OUT                 0x09
#define MMC5603NJ_STATUS_1              0x18
#define MMC5603NJ_ODR_CTRL              0x1A
#define MMC5603NJ_INTERNAL_CONTROL_0    0x1B
#define MMC5603NJ_INTERNAL_CONTROL_1    0x1C
#define MMC5603NJ_INTERNAL_CONTROL_2    0x1D
#define MMC5603NJ_ST_X_TH               0x1E
#define MMC5603NJ_ST_Y_TH               0x1F
#define MMC5603NJ_ST_Z_TH               0x20
#define MMC5603NJ_ST_X                  0x27
#define MMC5603NJ_ST_Y                  0x28
#define MMC5603NJ_ST_Z                  0x29
#define MMC5603NJ_PRODUCT_ID            0x39

#define MMC5603NJ_ID 0b00010000

// Sensitivity
#define MMC5603NJ_SENSITIVITY 0.0625f // 0.0625 milliGauss/LSB

class Mag_MMC5603NJ : public Sensor
{
private:
public:
    Mag_MMC5603NJ(I2C_HandleTypeDef *i2cHandler, Data *data);

    bool begin() override;
    void get_data() override;
};

#endif /* MAG_MMC5603NJ_H */

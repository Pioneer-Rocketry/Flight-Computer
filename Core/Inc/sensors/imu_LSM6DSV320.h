#ifndef IMU_LSM6DSV320_H
#define IMU_LSM6DSV320_H

#include "sensors/sensors.hpp"

// Address
#define LSM6DSV320_ADDRESS 0b1101010 << 1 // or 0b1101011 if 

// Register Address
#define LSM6DSV320_FUNC_CFG_ACCESS          0x01
#define LSM6DSV320_PIN_CTRL                 0x02
#define LSM6DSV320_IF_CFG                   0x03
/* Reserved 0x04 - 0x05 */
#define LSM6DSV320_ODR_TRIG_CFG             0x06
#define LSM6DSV320_FIFO_CTRL1               0x07
#define LSM6DSV320_FIFO_CTRL2               0x08
#define LSM6DSV320_FIFO_CTRL3               0x09
#define LSM6DSV320_FIFO_CTRL4               0x0A
#define LSM6DSV320_COUNTER_BDR_REG1         0x0B
#define LSM6DSV320_COUNTER_BDR_REG2         0x0C
#define LSM6DSV320_INT1_CTLR                0x0D
#define LSM6DSV320_INT2_CTLR                0x0E
#define LSM6DSV320_WHO_AM_I                 0x0F
#define LSM6DSV320_CTRL1                    0x10
#define LSM6DSV320_CTRL2                    0x11
#define LSM6DSV320_CTRL3                    0x12
#define LSM6DSV320_CTRL4                    0x13
#define LSM6DSV320_CTRL5                    0x14
#define LSM6DSV320_CTRL6                    0x15
#define LSM6DSV320_CTRL7                    0x16
#define LSM6DSV320_CTRL8                    0x17
#define LSM6DSV320_CTRL9                    0x18
#define LSM6DSV320_CTRL10                   0x19
#define LSM6DSV320_CTRL_STATUS              0x1A
#define LSM6DSV320_FIFO_STATUS1             0x1B
#define LSM6DSV320_FIFO_STATUS2             0x1C
#define LSM6DSV320_ALL_INT_REG              0x1D
#define LSM6DSV320_STATUS_REG               0x1E
/* Reserved 0x1F */
#define LSM6DSV320_OUT_TEMP_L               0x20
#define LSM6DSV320_OUT_TEMP_H               0x21
#define LSM6DSV320_OUTX_L_G                 0x22
#define LSM6DSV320_OUTX_H_G                 0x23
#define LSM6DSV320_OUTY_L_G                 0x24
#define LSM6DSV320_OUTY_H_G                 0x25
#define LSM6DSV320_OUTZ_L_G                 0x26
#define LSM6DSV320_OUTZ_H_G                 0x27
#define LSM6DSV320_OUTX_L_A                 0x28
#define LSM6DSV320_OUTX_H_A                 0x29
#define LSM6DSV320_OUTY_L_A                 0x2A
#define LSM6DSV320_OUTY_H_A                 0x2B
#define LSM6DSV320_OUTZ_L_A                 0x2C
#define LSM6DSV320_OUTZ_H_A                 0x2D
#define LSM6DSV320_UI_OUTX_L_G_OIS_EIS      0x2E
#define LSM6DSV320_UI_OUTX_H_G_OIS_EIS      0x2F
#define LSM6DSV320_UI_OUTY_L_G_OIS_EIS      0x30
#define LSM6DSV320_UI_OUTY_H_G_OIS_EIS      0x31
#define LSM6DSV320_UI_OUTZ_L_G_OIS_EIS      0x32
#define LSM6DSV320_UI_OUTZ_H_G_OIS_EIS      0x33
#define LSM6DSV320_UI_OUTX_L_A_OIS_HG       0x34
#define LSM6DSV320_UI_OUTX_H_A_OIS_HG       0x35
#define LSM6DSV320_UI_OUTY_L_A_OIS_HG       0x36
#define LSM6DSV320_UI_OUTY_H_A_OIS_HG       0x37
#define LSM6DSV320_UI_OUTZ_L_A_OIS_HG       0x38
#define LSM6DSV320_UI_OUTZ_H_A_OIS_HG       0x39
/* Reserved 0x3A - 0x3F */
#define LSM6DSV320_TIMESTAMP0               0x40
#define LSM6DSV320_TIMESTAMP1               0x41
#define LSM6DSV320_TIMESTAMP2               0x42
#define LSM6DSV320_TIMESTAMP3               0x43
#define LSM6DSV320_UI_STATUS_REG_OIS        0x44
#define LSM6DSV320_WAKE_UP_SRC              0x45
#define LSM6DSV320_TAP_SRC                  0x46
#define LSM6DSV320_D6D_SRC                  0x47
#define LSM6DSV320_STATUS_MASTER_MAINPAGE   0x48
#define LSM6DSV320_EMB_FUNC_MASTER_MAINPAGE 0x49
#define LSM6DSV320_FSM_MASTER_MAINPAGE      0x4A
#define LSM6DSV320_MLS_MASTER_MAINPAGE      0x4B
#define LSM6DSV320_HG_WAKE_UP_SRC           0x4C
#define LSM6DSV320_CTRL2_XL_HG              0x4D
#define LSM6DSV320_CTRL1_XL_HG              0x4E
#define LSM6DSV320_INTERNAL_FREQ_FINE       0x4F
#define LSM6DSV320_FUNCTIONS_ENABLE         0x50
/* Reserved 0x51 */
#define LSM6DSV320_HG_FUNCTIONS_ENABLE      0x52
#define LSM6DSV320_HG_WAKE_UP_THS           0x53
#define LSM6DSV320_INACTIVITY_DUR           0x54
#define LSM6DSV320_INACTIVITY_THS           0x55
#define LSM6DSV320_TAP_CFG0                 0x56
#define LSM6DSV320_TAP_CFG1                 0x57
#define LSM6DSV320_TAP_CFG2                 0x58
#define LSM6DSV320_TAP_THS_6D               0x59
#define LSM6DSV320_TAP_DUR                  0x5A
#define LSM6DSV320_WAKE_UP_THS              0x5B
#define LSM6DSV320_WAKE_UP_DUR              0x5C
#define LSM6DSV320_FREE_FALL                0x5D
#define LSM6DSV320_MD1_CFG                  0x5E
#define LSM6DSV320_MD2_CFG                  0x5F
/* Reserved 0x60 - 0x61 */
#define LSM6DSV320_HAODR_CFG                0x62
#define LSM6DSV320_EMB_FUNC_CTRL            0x63
#define LSM6DSV320_UI_HANDSHAKE_CTRL        0x64
#define LSM6DSV320_UI_SPI2_SHARED_0         0x65
#define LSM6DSV320_UI_SPI2_SHARED_1         0x66
#define LSM6DSV320_UI_SPI2_SHARED_2         0x67
#define LSM6DSV320_UI_SPI2_SHARED_3         0x68
#define LSM6DSV320_UI_SPI2_SHARED_4         0x69
#define LSM6DSV320_UI_SPI2_SHARED_5         0x6A
#define LSM6DSV320_CTRL_EIS                 0x6B
#define LSM6DSV320_XL_HG_X_OFS_USR          0x6C
#define LSM6DSV320_XL_HG_Y_OFS_USR          0x6D
#define LSM6DSV320_XL_HG_Z_OFS_USR          0x6E
#define LSM6DSV320_UI_INT_OIS               0x6F
#define LSM6DSV320_UI_CTRL1_OIS             0x70
#define LSM6DSV320_UI_CTRL2_OIS             0x71
#define LSM6DSV320_UI_CTRL3_OIS             0x72
#define LSM6DSV320_X_OFS_USR                0x73
#define LSM6DSV320_Y_OFS_USR                0x74
#define LSM6DSV320_Z_OFS_USR                0x75
/* Reserved 0x76 - 0x77 */
#define LSM6DSV320_FIFO_DATA_OUT_TAG        0x78
#define LSM6DSV320_FIFO_DATA_OUT_X_L        0x79
#define LSM6DSV320_FIFO_DATA_OUT_X_H        0x7A
#define LSM6DSV320_FIFO_DATA_OUT_Y_L        0x7B
#define LSM6DSV320_FIFO_DATA_OUT_Y_H        0x7C
#define LSM6DSV320_FIFO_DATA_OUT_Z_L        0x7D
#define LSM6DSV320_FIFO_DATA_OUT_Z_H        0x7E

#define LSM6DSV320_ID 0x73

// Sensitivity
#define LSM6DSV320_LOW_G_SENSITIVITY_2G  0.000061f  // +/- 2g  0.061 mg/LSB = 0.000061 g/LSB
#define LSM6DSV320_LOW_G_SENSITIVITY_4G  0.000122f  // +/- 4g  0.122 mg/LSB = 0.000122 g/LSB
#define LSM6DSV320_LOW_G_SENSITIVITY_8G  0.000244f  // +/- 8g  0.244 mg/LSB = 0.000244 g/LSB
#define LSM6DSV320_LOW_G_SENSITIVITY_16G 0.000488f  // +/- 16g 0.488 mg/LSB = 0.000488 g/LSB

#define LSM6DSV320_HIGH_G_SENSITIVITY_32G  0.000976f    // +/- 32g  1.952   mg/LSB = 0.000976 g/LSB
#define LSM6DSV320_HIGH_G_SENSITIVITY_64G  0.001952f    // +/- 64g  3.904   mg/LSB = 0.001952 g/LSB
#define LSM6DSV320_HIGH_G_SENSITIVITY_128G 0.003904f    // +/- 128g 7.808   mg/LSB = 0.003904 g/LSB
#define LSM6DSV320_HIGH_G_SENSITIVITY_256G 0.007808f    // +/- 256g 15.616  mg/LSB = 0.007808 g/LSB
#define LSM6DSV320_HIGH_G_SENSITIVITY_320G 0.010417f    // +/- 320g 20.833  mg/LSB = 0.010417 g/LSB

#define LSM6DSV320_GYRO_SENSITIVITY_250DPS  0.00875f    // +/- 250dgps  8.75 mdps/LSB = 0.00875 dps/LSB
#define LSM6DSV320_GYRO_SENSITIVITY_500DPS  0.0175f     // +/- 500dgps  17.5 mdps/LSB = 0.0175  dps/LSB
#define LSM6DSV320_GYRO_SENSITIVITY_1000DPS 0.0175f     // +/- 1000dgps 35   mdps/LSB = 0.0175  dps/LSB
#define LSM6DSV320_GYRO_SENSITIVITY_2000DPS 0.07f       // +/- 2000dgps 70   mdps/LSB = 0.07    dps/LSB
#define LSM6DSV320_GYRO_SENSITIVITY_4000DPS 0.14f       // +/- 4000dgps 140  mdps/LSB = 0.14    dps/LSB

#define GRAVITY             9.80665f
#define PI                  3.14159f
#define RADIANS_TO_DEGREES  PI/180.0f

class IMU_LSM6DSV320 : public Sensor {
private:

    // Low G Settings
    enum LSM6DSV320_LOW_G_RANGE { // Defined in datasheet on page 71, table 70.
        LOW_G_RANGE_2G    = 0b00,
        LOW_G_RANGE_4G    = 0b01,
        LOW_G_RANGE_8G    = 0b10,
        LOW_G_RANGE_16G   = 0b11
    };

    enum LSM6DSV320_LOW_G_OP_MODE { // Defined in datasheet on page 65, table 53.
        LOW_G_HIGH_PERFORMANCE  = 0b000, // High-performance mode (default)
        LOW_G_HIGH_ACCURACY     = 0b001, // High-accuracy ODR mode
                                         // Reserved
        LOW_G_ODR_TRIGGERED     = 0b011, // ODR-triggered mode
        LOW_G_LOW_POWER1        = 0b100, // Low-power mode 1
        LOW_G_LOW_POWER2        = 0b101, // Low-power mode 2
        LOW_G_LOW_POWER3        = 0b110, // Low-power mode 3
        LOW_G_NORMAL            = 0b111  // Normal
    };

    enum LSM6DSV320_LOW_G_ODR { // Defined in datasheet on page 65, table 54.
        LOW_G_ODR_POWER_DOWN    = 0b0000, // Power-down (default)
        LOW_G_ODR_1_875HZ       = 0b0001, // 1.875 Hz (low-power mode)
        LOW_G_ODR_7_5HZ         = 0b0010, // 7.5 Hz (high-performance, normal mode)
        LOW_G_ODR_15HZ          = 0b0011, // 15 Hz (low-power, high-performance, normal mode)
        LOW_G_ODR_30HZ          = 0b0100, // 30 Hz (low-power, high-performance, normal mode)
        LOW_G_ODR_60HZ          = 0b0101, // 60 Hz (low-power, high-performance, normal mode)
        LOW_G_ODR_120HZ         = 0b0110, // 120 Hz (low-power, high-performance, normal mode)
        LOW_G_ODR_240HZ         = 0b0111, // 240 Hz (low-power, high-performance, normal mode)
        LOW_G_ODR_480HZ         = 0b1000, // 480 Hz (high-performance, normal mode)
        LOW_G_ODR_960HZ         = 0b1001, // 960 Hz (high-performance, normal mode)
        LOW_G_ODR_1_92KHZ       = 0b1010, // 1.92 kHz (high-performance mode)
        LOW_G_ODR_7_68KHZ       = 0b1011  // 7.68 kHz (high-performance mode)
    };

    LSM6DSV320_LOW_G_RANGE lowGRange    = LOW_G_RANGE_16G;
    LSM6DSV320_LOW_G_OP_MODE lowGOPMode = LOW_G_HIGH_ACCURACY;
    LSM6DSV320_LOW_G_ODR lowGODR        = LOW_G_ODR_120HZ;

    // High G Settings
    enum LSM6DSV320_HIGH_G_RANGE { // Defined in datasheet on page 91, table 150.
        HIGH_G_RANGE_32G    = 0b000,
        HIGH_G_RANGE_64G    = 0b001,
        HIGH_G_RANGE_128G   = 0b010,
        HIGH_G_RANGE_256G   = 0b011,
        HIGH_G_RANGE_320G   = 0b100,
    };

    enum LSM6DSV320_HIGH_G_ODR { // Defined in datasheet on page 91, table 149.
        HIGH_G_ODR_POWER_DOWN   = 0b000, // Power-down (default)
                                         // Reserved
                                         // Reserved
        HIGH_G_ODR_480_HZ       = 0b011, // 480 Hz
        HIGH_G_ODR_960_HZ       = 0b100, // 960 Hz
        HIGH_G_ODR_1_92_KHZ     = 0b101, // 1.92 kHz
        HIGH_G_ODR_3_84_KHZ     = 0b110, // 3.84 kHz
        HIGH_G_ODR_7_68_KHZ     = 0b111, // 7.68 kHz
    };

    LSM6DSV320_HIGH_G_RANGE highGRange  = HIGH_G_RANGE_256G;
    LSM6DSV320_HIGH_G_ODR highGODR      = HIGH_G_ODR_480_HZ;

    // Gyro Settings
    enum LSM6DSV320_GYRO_RANGE { // Defined in datasheet on page 69, table 65.
        GYRO_RANGE_250DPS   = 0b001,
        GYRO_RANGE_500DPS   = 0b010,
        GYRO_RANGE_1000DPS  = 0b011,
        GYRO_RANGE_2000DPS  = 0b100,
        GYRO_RANGE_4000DPS  = 0b101
    };

    enum LSM6DSV320_GYRO_OP_MODE { // Defined in datasheet on page 65, table 53.
        GYRO_HIGH_PERFORMANCE   = 0b000, // High-performance mode (default)
        GYRO_HIGH_ACCURACY      = 0b001, // High-accuracy ODR mode
                                         // Reserved
        GYRO_ODR_TRIGGERED      = 0b011, // ODR-triggered mode
        GYRO_SLEEP              = 0b100, // Sleep mode
        GYRO_LOW_POWER          = 0b101, // Low-power mode
    };

    enum LSM6DSV320_GYRO_ODR { // Defined in datasheet on page 65, table 54.
        GYRO_ODR_POWER_DOWN    = 0b0000, // Power-down (default)
        GYRO_ODR_7_5HZ         = 0b0010, // 7.5 Hz (high-performance mode)
        GYRO_ODR_15HZ          = 0b0011, // 15 Hz (low-power, high-performance mode)
        GYRO_ODR_30HZ          = 0b0100, // 30 Hz (low-power, high-performance mode)
        GYRO_ODR_60HZ          = 0b0101, // 60 Hz (low-power, high-performance mode)
        GYRO_ODR_120HZ         = 0b0110, // 120 Hz (low-power, high-performance mode)
        GYRO_ODR_240HZ         = 0b0111, // 240 Hz (low-power, high-performance mode)
        GYRO_ODR_480HZ         = 0b1000, // 480 Hz (high-performance mode)
        GYRO_ODR_960HZ         = 0b1001, // 960 Hz (high-performance mode)
        GYRO_ODR_1_92KHZ       = 0b1010, // 1.92 kHz (high-performance mode)
        GYRO_ODR_3_58KHZ       = 0b1011, // 3.58 kHz (high-performance mode)
        GYRO_ODR_7_68KHZ       = 0b1100  // 7.68 kHz (high-performance mode)
    };

    LSM6DSV320_GYRO_RANGE gyroRange     = GYRO_RANGE_2000DPS;
    LSM6DSV320_GYRO_OP_MODE gyroOPMode  = GYRO_HIGH_ACCURACY;
    LSM6DSV320_GYRO_ODR gyroODR         = GYRO_ODR_120HZ;

    // Sensitivities
    float lowGSensitivity;
    float highGSensitivity;
    float gyroSensitivity;

public:
    IMU_LSM6DSV320(I2C_HandleTypeDef *i2cHandler, Data *data);

    bool begin() override;
    void get_data() override;
};

#endif /* IMU_LSM6DSV320_H */

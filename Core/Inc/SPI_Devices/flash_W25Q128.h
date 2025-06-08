#ifndef FLASH_W25Q128_H
#define FLASH_W25Q128_H

#include "stm32f4xx_hal.h"

#include "SPI_Devices/SPI_Device.hpp"
#include "data.h"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

// Defines

#define W25QXX_FLASH_PAGE_SIZE (uint8_t)256
#define W25QXX_FLASH_PAGES     (uint8_t)65536
#define W25QXX_FLASH_SIZE      (uint8_t)(W25QXX_FLASH_PAGES * W25QXX_FLASH_PAGE_SIZE)
#define W25QXX_FLASH_SECTORS   (uint8_t)4096
#define W25QXX_FLASH_BLOCKS    (uint8_t)256

#define WINBOND_MANUFACTURER_ID 0xEF
#define WINBOND_DEVICE_ID       0x17

// Addresses
#define W25QXX_WRITE_ENABLE             0x06
#define W25QXX_VOLATLE_SR_WRITE_ENABLE  0x50
#define W25QXX_WRITE_DISABLE            0x04
#define W25QXX_ID                       0xAB
#define W25QXX_DEVICE_ID                0x90
#define W25QXX_JEDEC_ID                 0x9F
#define W25QXX_UNIQUE_ID                0x4B
#define W25QXX_READ_DATA                0x03
#define W25QXX_FAST_READ                0x0B
#define W25QXX_PAGE_PROGRAM             0x02
#define W25QXX_SECTOR_ERASE             0x20
#define W25QXX_BLOCK_ERASE_32K          0x52
#define W25QXX_BLOCK_ERASE_64K          0xD8
#define W25QXX_CHIP_ERASE               0xC7
#define W25QXX_READ_STATUS_REG1         0x05
#define W25QXX_WRITE_STATUS_REG1        0x01
#define W25QXX_READ_STATUS_REG2         0x35
#define W25QXX_WRITE_STATUS_REG2        0x31
#define W25QXX_READ_STATUS_REG3         0x15
#define W25QXX_WRITE_STATUS_REG3        0x11
#define W25QXX_READ_SFDP_REG            0x5A
#define W25QXX_ERASE_SECURITY_REG       0x44
#define W25QXX_PROGRAM_SECURITY_REG     0x42
#define W25QXX_READ_SECURITY_REG        0x48
#define W25QXX_GLOBAL_BLOCK_LOCK        0x7E
#define W25QXX_GLOBAL_BLOCK_UNLOCK      0x98
#define W25QXX_READ_BLOCK_LOCK          0x3D
#define W25QXX_INDIVIDUAL_BLOCK_LOCK    0x36
#define W25QXX_INDIVIDUAL_BLOCK_UNLOCK  0x39
#define W25QXX_SUSPEND                  0x75
#define W25QXX_RESUME                   0x7A
#define W25QXX_POWER_DOWN               0xB9
#define W25QXX_ENABLE_RESET             0x66
#define W25QXX_RESET_DEVICE             0x99
#define W25QXX_FAST_READ_DUAL_OUTPUT    0x3B
#define W25QXX_FAST_READ_DUAL_IO        0xBB
#define W25QXX_DEVICE_ID_DUAL_IO        0x92
#define W25QXX_QUAD_INPUT_PAGE_PROGRAM  0x32
#define W25QXX_FAST_READ_QUAD_OUTPUT    0x6B
#define W25QXX_DEVICE_ID_QUAD_IO        0x94
#define W25QXX_FAST_READ_QUAD_IO        0xEB
#define W25QXX_SET_BURST_WRAP           0x77

struct datapoint_t {
    // Raw Sensor Readings
    float LSM6DSV320_LowG_Accel_X, LSM6DSV320_LowG_Accel_Y, LSM6DSV320_LowG_Accel_Z;
    float LSM6DSV320_HighG_Accel_X, LSM6DSV320_HighG_Accel_Y, LSM6DSV320_HighG_Accel_Z;
    float LSM6DSV320_Gyro_X, LSM6DSV320_Gyro_Y, LSM6DSV320_Gyro_Z;

    float MMC5603_Mag_X, MMC5603_Mag_Y, MMC5603_Mag_Z;

    float MS5607_Pressure;
    float MS5607_Temperature;
    float MS5607_Altitude;

    float GPS_Latitude, GPS_Longitude, GPS_Altitude;

    // State Estimation
    float orientation_w, orientation_x, orientation_y, orientation_z;
    float position_x, position_y, position_z;
    float velocity_x, velocity_y, velocity_z;

    uint32_t timestamp;

    // Pyro Channels
    // float Pyro1_Volt, Pyro2_Volt, Pyro3_Volt;
    // float Pyro_Volt;
    bool Pyro_State; // 1 bit for each channel continuity, 1 bit for each channel has been triggered, 1 bit for if pyros are armed

    uint8_t state;
};

#define STORE_NUM_DATAPOINTS 2

/**
 * Flash_W25Q128 storage class
 */
class Flash_W25Q128 : public SPI_Device {
private:
    uint8_t* buffer[W25QXX_FLASH_PAGE_SIZE + 3]; // 3 bytes for address
    uint8_t len = 0;
    uint32_t readAddr = 0;
    uint32_t writeAddr = 0;
    datapoint_t datapoint;

    const uint8_t DATA_POINT_SIZE = sizeof(datapoint_t);

public:
    Flash_W25Q128(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

    bool begin() override;
    void loop() override;

    void print();
};


#endif /* FLASH_W25Q128_H */
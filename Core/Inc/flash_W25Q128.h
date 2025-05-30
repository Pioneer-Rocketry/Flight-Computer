// #ifndef FLASH_W25Q128_H
// #define FLASH_W25Q128_H

// #include "stm32f4xx_hal.h"

// #include "data.h"

// // Defines

// #define W25QXX_FLASH_PAGE_SIZE 256
// #define W25QXX_FLASH_PAGES     65536
// #define W25QXX_FLASH_SIZE      (W25QXX_FLASH_PAGES * W25QXX_FLASH_PAGE_SIZE)
// #define W25QXX_FLASH_SECTORS   4096
// #define W25QXX_FLASH_BLOCKS    256

// #define WINBOND_MANUFACTURER_ID 0xEF
// #define WINBOND_DEVICE_ID       0x17

// // Addresses

// #define W25QXX_WRITE_ENABLE             0x06
// #define W25QXX_VOLATLE_SR_WRITE_ENABLE  0x50
// #define W25QXX_WRITE_DISABLE            0x04

// #define W25QXX_ID           0xAB
// #define W25QXX_DEVICE_ID    0x90
// #define W25QXX_JEDEC_ID     0x9F
// #define W25QXX_UNIQUE_ID    0x4B

// #define W25QXX_READ         0x03
// #define W25QXX_FAST_READ    0x0B

// #define W25QXX_PAGE_PROGRAM 0x02

// #define W25QXX_SECTOR_ERASE     0x20
// #define W25QXX_BLOCK_ERASE_32K  0x52
// #define W25QXX_BLOCK_ERASE_64K  0xD8
// #define W25QXX_CHIP_ERASE       0xC7

// #define W25QXX_READ_STATUS_REG_1    0x05
// #define W25QXX_WRITE_STATUS_REG_1   0x01
// #define W25QXX_READ_STATUS_REG_2    0x35
// #define W25QXX_WRITE_STATUS_REG_2   0x31
// #define W25QXX_READ_STATUS_REG_3    0x15
// #define W25QXX_WRITE_STATUS_REG_3   0x11

// #define W25QXX_READ_SFDP_REG        0x5A
// #define W25QXX_ERASE_SECURITY_REG   0x44
// #define W25QXX_PROGRAM_SECURITY_REG 0x42
// #define W25QXX_READ_SECURITY_REG    0x48

// #define W25QXX_GLOBAL_BLOCK_LOCK        0x7E
// #define W25QXX_GLOBAL_BLOCK_UNLOCK      0x98
// #define W25QXX_READ_BLOCK_LOCK          0x3D
// #define W25QXX_INDIVIDUAL_BLOCK_LOCK    0x36
// #define W25QXX_INDIVIDUAL_BLOCK_UNLOCK  0x39

// #define W25QXX_SUSPEND      0x75
// #define W25QXX_RESUME       0x7A
// #define W25QXX_POWER_DOWN   0xB9

// #define W25QXX_ENABLE_RESET 0x66
// #define W25QXX_RESET_DEVICE 0x99

// #define W25QXX_FAST_READ_DUAL_OUTPUT    0x3B
// #define W25QXX_FAST_READ_DUAL_IO        0xBB
// #define W25QXX_DEVICE_ID_DUAL_IO        0x92
// #define W25QXX_QUAD_INPUT_PAGE_PROGRAM  0x32
// #define W25QXX_FAST_READ_QUAD_OUTPUT    0x6B
// #define W25QXX_DEVICE_ID_QUAD_IO        0x94
// #define W25QXX_FAST_READ_QUAD_IO        0xEB
// #define W25QXX_SET_BURST_WRAP           0x77

// /**
//  * Flash_W25Q128 storage class
//  */
// class Flash_W25Q128 {
// private:
//     Data *data;

//     SPI_HandleTypeDef *spi;

//     uint32_t current_address;

//     bool write(uint8_t buffer[], uint32_t size=W25QXX_FLASH_PAGE_SIZE);

//     struct {
//         Data data;
//         uint8_t raw_data[];
//     } data_to_bytes;


// public:
//     bool begin();
//     bool write_data();

//     Flash_W25Q128(SPI_HandleTypeDef *spi, Data *data);
// };


// #endif /* FLASH_W25Q128_H */
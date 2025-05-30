// #include "flash_W25Q128.h"

// Flash_W25Q128::Flash_W25Q128(SPI_HandleTypeDef *spi, Data *data) {
//     this->spi  = spi;
//     this->data = data;
// }

// bool Flash_W25Q128::begin() {
//     // Check Manufacturer ID

//     uint8_t command[2];

//     command[0] = W25QXX_JEDEC_ID;
//     uint8_t jedec_id;
//     HAL_SPI_TransmitReceive(spi, command, &jedec_id, 1, HAL_MAX_DELAY);

//     if (jedec_id != WINBOND_MANUFACTURER_ID) {
//         return false;
//     }

//     // Write to status registers

//     // Status Register 1
//     command[0] = W25QXX_WRITE_STATUS_REG_1;
//     command[1] = 0x00;
//     command[1] |=   0b0 << 7; // Status Register Protect - SRP
//     command[1] |=   0b0 << 6; // Sector Protect - SEC
//     command[1] |= 0b000 << 3; // Top/Bottom Protect - TB
//     command[1] |=   0b0 << 2; // Block Protect Bits - BPx
//     command[1] |=   0b0 << 1; // Write Enable Latch - WEL (Read only)
//     command[1] |=   0b0 << 0; // Write in Progress - BUSY (Read only)

//     HAL_SPI_Transmit(spi, command, 2, HAL_MAX_DELAY);

//     // Status Register 2
//     command[0] = W25QXX_WRITE_STATUS_REG_2;
//     command[1] = 0x00;
//     command[1] |=   0b0 << 7; // Suspend Status - SUS (Read only)
//     command[1] |=   0b0 << 6; // Complement Protect - CMP
//     command[1] |= 0b000 << 3; // Security Register Lock Bits - LBx
//     command[1] |=   0b0 << 2; // Reserved
//     command[1] |=   0b0 << 1; // Quad Enable - QE
//     command[1] |=   0b0 << 0; // Status Registeer Lock - SRL

//     HAL_SPI_Transmit(spi, command, 2, HAL_MAX_DELAY);

//     // Status Register 3
//     command[0] = W25QXX_WRITE_STATUS_REG_3;
//     command[1] = 0x00;
//     command[1] |=   0b0 << 7; // /HOLD - HOLD
//     command[1] |=  0b01 << 5; // Output Driver Strength - DRVn
//     command[1] |=  0b00 << 3; // Reserved
//     command[1] |=   0b0 << 2; // Write Protect Select - WPS
//     command[1] |=  0b00 << 0; // Reserved

//     HAL_SPI_Transmit(spi, command, 2, HAL_MAX_DELAY);

//     return true;
// }

// /**
//  * Write data to the flash storage
//  */
// bool Flash_W25Q128::write_data() {
//     this->data_to_bytes.data = *data;

//     return write(data_to_bytes.raw_data, sizeof(*data));
// }

// /**
//  * Write a list of bytes to the flash storage
//  *
//  * @param buffer The data to be written
//  * @param size The number of bytes to be written
//  */
// bool Flash_W25Q128::write(uint8_t buffer[], uint32_t size) {
//     if (size > W25QXX_FLASH_PAGE_SIZE) {
//         return false; // You can only write a max of 1 page (256 bytes) of data
//     }

//     uint8_t command[4 + size];

//     // Set write enable
//     command[0] = W25QXX_WRITE_ENABLE;
//     HAL_SPI_Transmit(spi, command, 1, HAL_MAX_DELAY);

//     // Write data
//     command[0] = W25QXX_PAGE_PROGRAM;
//     command[1] = current_address >> 16;
//     command[2] = current_address >> 8;
//     command[3] = current_address;
//     for (uint32_t i = 0; i < size; i++) {
//         command[4 + i] = buffer[i];
//     }

//     HAL_SPI_Transmit(spi, command, size + 4, HAL_MAX_DELAY);

//     current_address += size;

//     return true;
// }
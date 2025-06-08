#include "SPI_Devices/flash_W25Q128.h"

Flash_W25Q128::Flash_W25Q128(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPI_Device(data, spi, port, pin, "W25Q128") {
}


bool Flash_W25Q128::begin() {
    uint8_t deviceID = 0;

    read_SPI(W25QXX_DEVICE_ID, &deviceID);
    if (deviceID != WINBOND_DEVICE_ID) {
        return false;
    }

    /* Device configuration */
    uint8_t config = 0;

    write_SPI(W25QXX_WRITE_ENABLE, nullptr); // Send Write Enable command
    write_SPI(W25QXX_VOLATLE_SR_WRITE_ENABLE, nullptr); // Send Volatile Status Register Write Enable command

    // Status Register 1
    config = 0;
    config |= 0b0   << 7; // Status Register Protect
    config |= 0b0   << 6; // Sector/Block Protect
    config |= 0b0   << 5; // Top/Bottom Block Protect
    config |= 0b000 << 4; // Block Protect Bits
    config |= 0b0   << 1; // Write Enable Latch (Read Only)
    config |= 0b0   << 0; // Erase/Write in Progress (Read Only)
    write_SPI(W25QXX_WRITE_STATUS_REG1, &config);

    // Status Register 2
    config = 0;
    config |= 0b0   << 7; // Erase/Program Suspend Status (Read Only)
    config |= 0b0   << 6; // Complement Protect
    config |= 0b000 << 5; // Security Register Lock Bits
    config |= 0b0   << 2; // Reserved
    config |= 0b0   << 1; // Quad Enable
    config |= 0b0   << 0; // Status register lock
    write_SPI(W25QXX_WRITE_STATUS_REG2, &config);

    // Status Register 3
    config = 0;
    config |= 0b0  << 7; // Set pin 7 to /HOLD or to /RESET
    config |= 0b11 << 6; // Output Driver Strength
    config |= 0b0  << 4; // Reserved
    config |= 0b0  << 3; // Reserved
    config |= 0b0  << 2; // Write Protect Selection
    config |= 0b0  << 1; // Reserved
    config |= 0b0  << 0; // Reserved
    write_SPI(W25QXX_WRITE_STATUS_REG3, &config);

    return true;
}

void Flash_W25Q128::loop() {
    datapoint.LSM6DSV320_LowG_Accel_X = data->LSM6DSV320_LowG_Accel.x;
    datapoint.LSM6DSV320_LowG_Accel_Y = data->LSM6DSV320_LowG_Accel.y;
    datapoint.LSM6DSV320_LowG_Accel_Z = data->LSM6DSV320_LowG_Accel.z;

    datapoint.LSM6DSV320_HighG_Accel_X = data->LSM6DSV320_HighG_Accel.x;
    datapoint.LSM6DSV320_HighG_Accel_Y = data->LSM6DSV320_HighG_Accel.y;
    datapoint.LSM6DSV320_HighG_Accel_Z = data->LSM6DSV320_HighG_Accel.z;

    datapoint.LSM6DSV320_Gyro_X = data->LSM6DSV320_Gyro.x;
    datapoint.LSM6DSV320_Gyro_Y = data->LSM6DSV320_Gyro.y;
    datapoint.LSM6DSV320_Gyro_Z = data->LSM6DSV320_Gyro.z;

    datapoint.MMC5603_Mag_X = data->MMC5603_Mag.x;
    datapoint.MMC5603_Mag_Y = data->MMC5603_Mag.y;
    datapoint.MMC5603_Mag_Z = data->MMC5603_Mag.z;

    datapoint.MS5607_Pressure = data->MS5607_Pressure;
    datapoint.MS5607_Altitude = data->MS5607_Altitude;
    datapoint.MS5607_Temperature = data->MS5607_Temperature;

    // datapoint.GPS_Latitude = data->GPS_Latitude;
    // datapoint.GPS_Longitude = data->GPS_Longitude;
    // datapoint.GPS_Altitude = data->GPS_Altitude;

    // datapoint.Pyro_State = data->Pyro_State;

    datapoint.orientation_w = data->orientation.w;
    datapoint.orientation_x = data->orientation.x;
    datapoint.orientation_y = data->orientation.y;
    datapoint.orientation_z = data->orientation.z;

    datapoint.position_x = data->position.x;
    datapoint.position_y = data->position.y;
    datapoint.position_z = data->position.z;

    datapoint.velocity_x = data->velocity.x;
    datapoint.velocity_y = data->velocity.y;
    datapoint.velocity_z = data->velocity.z;

    datapoint.timestamp = data->timestamp;

    // Check to see if buffer is full, if not add datapoint to it
    if (len + DATA_POINT_SIZE < W25QXX_FLASH_PAGE_SIZE) {
        memcpy(&buffer[len], (char*)&datapoint, DATA_POINT_SIZE);

        len += DATA_POINT_SIZE;
        writeAddr += DATA_POINT_SIZE;
    } else {
        write_SPI(W25QXX_PAGE_PROGRAM, *buffer, W25QXX_FLASH_PAGE_SIZE+3);

        // Clear buffer
        for (int i = 0; i < W25QXX_FLASH_PAGE_SIZE; i++) buffer[i] = 0;
        len = 0;

        // Copy 3 bytes of the address into the buffer
        buffer[0] = (uint8_t*)(writeAddr >> 16 & 0xFF);
        buffer[1] = (uint8_t*)(writeAddr >> 8 & 0xFF);
        buffer[2] = (uint8_t*)(writeAddr & 0xFF);
    }
}

void Flash_W25Q128::print() {
    readAddr = 0;

    // Loop over every page and decode it and turn it into data and print data
    for (uint8_t i = 0; i < W25QXX_FLASH_PAGES; i++) {
        for (int x = 0; x < W25QXX_FLASH_PAGE_SIZE; x++) buffer[x] = 0; // Clear buffer
        // Copy 3 bytes of the address into the buffer
        readAddr += W25QXX_FLASH_PAGE_SIZE;
        buffer[0] = (uint8_t*)(readAddr >> 16 & 0xFF);
        buffer[1] = (uint8_t*)(readAddr >> 8 & 0xFF);
        buffer[2] = (uint8_t*)(readAddr & 0xFF);

        read_SPI(W25QXX_READ_DATA, *buffer, W25QXX_FLASH_PAGE_SIZE);

        // Convert the page into datapoint (most likly 2 datapoints per page)
        for (int x = 0; x < W25QXX_FLASH_PAGE_SIZE; x += DATA_POINT_SIZE) {
            memcpy(&datapoint, &buffer[x], DATA_POINT_SIZE);

            this->data->LSM6DSV320_LowG_Accel.x = datapoint.LSM6DSV320_LowG_Accel_X;
            this->data->LSM6DSV320_LowG_Accel.y = datapoint.LSM6DSV320_LowG_Accel_Y;
            this->data->LSM6DSV320_LowG_Accel.z = datapoint.LSM6DSV320_LowG_Accel_Z;

            this->data->LSM6DSV320_HighG_Accel.x = datapoint.LSM6DSV320_HighG_Accel_X;
            this->data->LSM6DSV320_HighG_Accel.y = datapoint.LSM6DSV320_HighG_Accel_Y;
            this->data->LSM6DSV320_HighG_Accel.z = datapoint.LSM6DSV320_HighG_Accel_Z;

            this->data->LSM6DSV320_Gyro.x = datapoint.LSM6DSV320_Gyro_X;
            this->data->LSM6DSV320_Gyro.y = datapoint.LSM6DSV320_Gyro_Y;
            this->data->LSM6DSV320_Gyro.z = datapoint.LSM6DSV320_Gyro_Z;

            this->data->MMC5603_Mag.x = datapoint.MMC5603_Mag_X;
            this->data->MMC5603_Mag.y = datapoint.MMC5603_Mag_Y;
            this->data->MMC5603_Mag.z = datapoint.MMC5603_Mag_Z;

            this->data->MS5607_Pressure = datapoint.MS5607_Pressure;
            this->data->MS5607_Altitude = datapoint.MS5607_Altitude;
            this->data->MS5607_Temperature = datapoint.MS5607_Temperature;

            this->data->orientation.w = datapoint.orientation_w;
            this->data->orientation.x = datapoint.orientation_x;
            this->data->orientation.y = datapoint.orientation_y;
            this->data->orientation.z = datapoint.orientation_z;

            this->data->position.x = datapoint.position_x;
            this->data->position.y = datapoint.position_y;
            this->data->position.z = datapoint.position_z;

            this->data->velocity.x = datapoint.velocity_x;
            this->data->velocity.y = datapoint.velocity_y;
            this->data->velocity.z = datapoint.velocity_z;

            this->data->timestamp = datapoint.timestamp;
            this->data->state = datapoint.state;

            // datapoint.GPS_Latitude = data->GPS_Latitude;
            // datapoint.GPS_Longitude = data->GPS_Longitude;
            // datapoint.GPS_Altitude = data->GPS_Altitude;

            // datapoint.Pyro_State = data->Pyro_State;

            this->data->log();
        }
    }
}
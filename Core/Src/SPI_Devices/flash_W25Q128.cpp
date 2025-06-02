#include "SPI_Devices/flash_W25Q128.h"

Flash_W25Q128::Flash_W25Q128(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPI_Device(data, spi, port, pin, "W25Q128") {
}


bool Flash_W25Q128::begin() {
    return true;
}

void Flash_W25Q128::loop() {
}
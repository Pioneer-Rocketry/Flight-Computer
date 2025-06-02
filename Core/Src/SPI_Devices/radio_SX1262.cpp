#include "SPI_Devices/radio_SX1262.h"

Radio_SX1262::Radio_SX1262(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPI_Device(data, spi, port, pin, "SX1262") {
}

bool Radio_SX1262::begin() {
    return true;
}

void Radio_SX1262::loop() {
}
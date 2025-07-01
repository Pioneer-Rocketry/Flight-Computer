#include "SPI_Devices/radio_RFM95W.h"

Radio_RFM95W::Radio_RFM95W(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPI_Device(data, spi, port, pin, "RFM95W") {
}

bool Radio_RFM95W::begin() {
    return true;
}

void Radio_RFM95W::loop() {
}
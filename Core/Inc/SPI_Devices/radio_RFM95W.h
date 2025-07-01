#ifndef RADIO_RFM95W_H
#define RADIO_RFM95W_H

#include "stm32f4xx_hal.h" /* Needed for SPI */

#include "SPI_Devices/SPI_Device.hpp"
#include "data.h"

// Registers

class Radio_RFM95W : public SPI_Device
{
private:

    void writeRegister(uint16_t reg, uint8_t value);
    uint8_t readRegister(uint16_t reg);

public:
    Radio_RFM95W(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

    bool begin() override;
    void loop() override;
};

#endif /* RADIO_RFM95W_H */

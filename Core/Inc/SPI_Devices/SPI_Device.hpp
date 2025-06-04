#ifndef SPI_DEVICE_HPP
#define SPI_DEVICE_HPP

#include <string.h> /* Needed for memcpy */

#include "micros.h"

#include "stm32f4xx_hal.h" /* Needed for spi */
#include "data.h"

/**
 * SPI Device Abstraction Class
 *
 * The purpose of this class is to allow the spi devices to be used in a generic way
 * Every spi device will inherit from this class
 */
class SPI_Device {
private:
    char name[32];

protected:
    SPI_HandleTypeDef *spiHandler;

    GPIO_TypeDef *chipSelectPort;
    uint16_t chipSelectPin;

    HAL_StatusTypeDef status;

    Data *data;

    /**
     * Read data from the spi bus
     *
     * @param reg The register to read from
     * @param data The data to be read
     * @param len The number of bytes to read
     */
    HAL_StatusTypeDef read_SPI(uint8_t reg, uint8_t *data, uint8_t len=1) {
        uint8_t tx[len+1];
        uint8_t rx[len+1];

        tx[0] = reg;
        for (int i=1; i < len+1; i++) tx[i] = 0x00;
        for (int i=0; i < len+1; i++) rx[i] = 0x00;

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

        // status = HAL_SPI_Transmit(this->spiHandler, &reg, len, 10);
        // status = HAL_SPI_Receive(this->spiHandler, data, len, 10);
        // status = HAL_SPI_TransmitReceive(this->spiHandler, &reg, data, len+1, 10);
        status = HAL_SPI_TransmitReceive(this->spiHandler, tx, rx, len+1, 10);

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

        memcpy(data, rx + 1, len);

        return status;
    }

    /**
     * Write data to the spi bus
     *
     * @param reg The register to write to
     * @param data The data to be written
     * @param len The number of bytes to write
     */
    HAL_StatusTypeDef write_SPI(uint8_t reg, uint8_t *data, uint8_t len=1) {
        uint8_t buffer[len + 1];
        buffer[0] = reg;
        memcpy(buffer + 1, data, len);

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

        status = HAL_SPI_Transmit(this->spiHandler, buffer, len+1, 10);

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

        return status;
    }

public:
    virtual bool begin() = 0;
    virtual void loop() = 0;

    /**
     * Constructor
     *
     * @param data The data struct
     * @param spi The spi handler
     * @param port The chip select port
     * @param pin The chip select pin
     * @param name The name of the device
     */
    SPI_Device(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin, char name[32]) {
        this->data = data;
        this->spiHandler = spi;
        this->chipSelectPort = port;
        this->chipSelectPin = pin;

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

        memcpy(this->name, name, sizeof(this->name));
    }

    /**
     * Get the name of the sensor
     *
     * @return The name of the sensor
     */
    char* get_name() {
        return this->name;
    }
};

#endif /* SPI_DEVICE_HPP */

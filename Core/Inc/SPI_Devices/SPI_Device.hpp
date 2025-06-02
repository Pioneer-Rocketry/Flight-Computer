#ifndef SPI_DEVICE_HPP
#define SPI_DEVICE_HPP

#include <string.h> /* Needed for memcpy */

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
    SPI_HandleTypeDef *spiHandler;
    HAL_StatusTypeDef status;

    GPIO_TypeDef *chipSelectPort;
    uint16_t chipSelectPin;

    char name[32];

protected:
    Data *data;

    /**
     * Read data from the spi bus
     *
     * @param reg The register to read from
     * @param data The data to be read
     * @param len The number of bytes to read
     */
    HAL_StatusTypeDef read_SPI(uint8_t reg, uint8_t *data, uint8_t len=1) {
        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

        status = HAL_SPI_Transmit(this->spiHandler, &reg, len, HAL_MAX_DELAY);
        status = HAL_SPI_Receive(this->spiHandler, data, len, HAL_MAX_DELAY);

        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_SET);

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
        HAL_GPIO_WritePin(this->chipSelectPort, this->chipSelectPin, GPIO_PIN_RESET);

        status = HAL_SPI_Transmit(this->spiHandler, &reg, len, HAL_MAX_DELAY);

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

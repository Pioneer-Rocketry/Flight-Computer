
#ifndef SENSOR_H
#define SENSOR_H

#include "stm32f4xx_hal.h" /* Needed for i2c */

#include "data.h"

/**
 * Sensor Abstraction Class
 *
 * The purpose of this class is to allow the sensors to be used in a generic way
 * Every sensor will inherit from this class
 */

class Sensor {
private:
    I2C_HandleTypeDef *i2cHandler;

    uint8_t address;

protected:
    Data *data;

    /**
     * Read data from the i2c bus
     *
     * @param reg The register to read from
     * @param data The data to be read
     * @param len The number of bytes to read
     */
    HAL_StatusTypeDef read_I2C(uint16_t reg, uint8_t *data, uint16_t len=1) {
        return HAL_I2C_Mem_Read(this->i2cHandler, this->address, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    }

    /**
     * Write data to the i2c bus
     *
     * @param reg The register to write to
     * @param data The data to be written
     * @param len The number of bytes to write
     */
    HAL_StatusTypeDef write_I2C(uint8_t reg, uint8_t *data, uint8_t len=1) {
        return HAL_I2C_Mem_Write(this->i2cHandler, this->address, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    }

    /**
     * Get the address of the sensor
     *
     * @return The address of the sensor
     */
    uint8_t get_address() {
        return this->address;
    }

public:
    virtual bool begin() = 0;
    virtual void get_data() = 0;

    Sensor(I2C_HandleTypeDef *i2cHandler, uint8_t address, Data *data) {
        this->i2cHandler = i2cHandler;
        this->address = address;
        this->data = data;
    }
};

#endif
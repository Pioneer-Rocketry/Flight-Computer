#include "I2C_Devices/baro_MS5607.h"


Baro_MS5607::Baro_MS5607(I2C_HandleTypeDef* i2cHandler, Data* data)
    : I2C_Device(i2cHandler, MS5607_ADDRESS, data, "MS5607") {
}

bool Baro_MS5607::begin() {
    if (write_I2C(MS5607_RESET, nullptr, 0) != HAL_OK) {
        return false;
    }

    HAL_Delay(3); // Max reset time

    switch (osr) {
        case OSR_256:   conversion_time = 1; break;
        case OSR_512:   conversion_time = 1; break;
        case OSR_1024:  conversion_time = 2; break;
        case OSR_2048:  conversion_time = 5; break;
        case OSR_4096:  conversion_time = 9; break;
        default: return false;
    }

    return read_prom();
}

bool Baro_MS5607::read_prom() {
    for (uint8_t i = 0; i < 7; i++) {
        if (read_I2C(MS5607_PROM_READ + (i * 2), buf, 2) != HAL_OK)
            return false;
        C[i] = (buf[0] << 8) | buf[1];
    }
    return true;
}

uint32_t Baro_MS5607::read_adc(uint8_t cmd) {
    write_I2C(cmd, nullptr, 0);
    HAL_Delay(conversion_time);

    read_I2C(MS5607_ADC_READ, buf, 3);
    return (buf[0] << 16) | (buf[1] << 8) | buf[2];
}

void Baro_MS5607::loop() {
    D1 = read_adc(MS5607_CONVERT_D1 | (osr << 1));
    D2 = read_adc(MS5607_CONVERT_D2 | (osr << 1));

    dT = D2 - ((uint32_t)C[5] << 8);
    TEMP = 2000 + ((int64_t)dT * C[6]) / (1 << 23);

    OFF = ((int64_t)C[2] << 17) + (((int64_t)C[4] * dT) >> 6);
    SENS = ((int64_t)C[1] << 16) + (((int64_t)C[3] * dT) >> 7);

    P = (((D1 * SENS) >> 21) - OFF) >> 15;

    if (this->data->startingAltitude == 0)
        this->data->startingAltitude = this->data->MS5607_Altitude;

    this->data->MS5607_Temperature = TEMP / 100.0f;
    this->data->MS5607_Pressure = P / 100.0f;
    this->data->MS5607_Altitude = (1-pow(this->data->MS5607_Pressure/1013.25, 0.190284))*145366.45 * FT_TO_M;
}

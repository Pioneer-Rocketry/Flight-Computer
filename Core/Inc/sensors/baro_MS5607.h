#ifndef BARO_MS5607_H
#define BARO_MS5607_H

#include "sensors/sensors.hpp"

// Address
#define MS5607_ADDRESS 0b1110111 << 1 // or 0b1110111 if CSB is high

// Commands
#define MS5607_RESET        0x1E
#define MS5607_CONVERT_D1   0x40 // Pressure conversion
#define MS5607_CONVERT_D2   0x50 // Temperature conversion
#define MS5607_ADC_READ     0x00
#define MS5607_PROM_READ    0xA0 // Base address for PROM read

class Baro_MS5607 : public Sensor {
private:
    uint16_t C[7]; // Calibration coefficients

    uint32_t D1, D2;
    int32_t dT, TEMP, P;
    int64_t OFF, SENS;

    uint8_t buf[3];

    bool read_prom();
    uint32_t read_adc(uint8_t cmd);

    enum Baro_MS5607_OSR  {
        OSR_256  = 0b000,
        OSR_512  = 0b001,
        OSR_1024 = 0b010,
        OSR_2048 = 0b011,
        OSR_4096 = 0b100
    } osr = OSR_256;

    int conversion_time;

public:
    Baro_MS5607(I2C_HandleTypeDef* i2cHandler, Data* data);

    bool begin() override;
    void get_data() override;
};

#endif /* BARO_MS5607_H */
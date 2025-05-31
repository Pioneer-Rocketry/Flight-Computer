
#include "data.h"

Data::Data(UART_HandleTypeDef* uart) {
    this->uart = uart;

    // Zero all data
    this->LSM6DSV320_LowG_Accel.zero();
    this->LSM6DSV320_HighG_Accel.zero();
    this->LSM6DSV320_Gyro.zero();
    this->MMC5603_Mag.zero();

    this->MS5607_Pressure = 0;
    this->MS5607_Altitude = 0;
}

void Data::save() {
}

void Data::log() {
    int len = snprintf(buffer, sizeof(buffer),
        "Timestamp: %d\r\n"
        "Frequency: %.02f Hz\r\n"
        "LowG Accel:    X: %.2f Y: %.2f Z: %.2f m/s^2\r\n"
        "HighG Accel:   X: %.2f Y: %.2f Z: %.2f m/s^2\r\n"
        "Gyro:          X: %.2f Y: %.2f Z: %.2f deg/s\r\n"
        "Mag:           X: %.2f Y: %.2f Z: %.2f mG\r\n"
        "Pressure: %.2f hPa | Altitude: %.2f m\r\n"
        "Orientation:   W: %.2f X: %.2f Y: %.2f Z: %.2f\r\n"
        "Position:      X: %.2f Y: %.2f Z: %.2f m\r\n"
        "Velocity:      X: %.2f Y: %.2f Z: %.2f m/s\r\n\r\n",
        this->timestamp, this->freq,
        LSM6DSV320_LowG_Accel.x, LSM6DSV320_LowG_Accel.y, LSM6DSV320_LowG_Accel.z,
        LSM6DSV320_HighG_Accel.x, LSM6DSV320_HighG_Accel.y, LSM6DSV320_HighG_Accel.z,
        LSM6DSV320_Gyro.x, LSM6DSV320_Gyro.y, LSM6DSV320_Gyro.z,
        MMC5603_Mag.x, MMC5603_Mag.y, MMC5603_Mag.z,
        MS5607_Pressure, MS5607_Altitude,
        orientation.w, orientation.x, orientation.y, orientation.z,
        position.x, position.y, position.z,
        velocity.x, velocity.y, velocity.z
    );

    if (len > 0) {
        HAL_UART_Transmit(uart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
    }
}
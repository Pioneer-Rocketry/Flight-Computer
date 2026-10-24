#include "Subsystems/Navigation.h"

Navigation::Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus,
                       UART_HandleTypeDef* uart, uint8_t* gpsRxBuffer)
    : Subsystem(data),
      imu(data, spiBus, IMU_CS_GPIO_Port, IMU_CS_Pin),
      baro(data, spiBus, BARO_CS_GPIO_Port, BARO_CS_Pin),
      gps(data, uart, gpsRxBuffer),
      lastLoop_us(micros())
{
    publish();
}

NavigationSample Navigation::sample() const
{
    NavigationSample s{};
    s.low_g[0] = data->LSM6DSV320LowGAccelX_mps2;
    s.low_g[1] = data->LSM6DSV320LowGAccelY_mps2;
    s.low_g[2] = data->LSM6DSV320LowGAccelZ_mps2;
    s.high_g[0] = data->LSM6DSV320HighGAccelX_mps2;
    s.high_g[1] = data->LSM6DSV320HighGAccelY_mps2;
    s.high_g[2] = data->LSM6DSV320HighGAccelZ_mps2;
    s.gyro_dps[0] = data->LSM6DSV320GyroX_dps;
    s.gyro_dps[1] = data->LSM6DSV320GyroY_dps;
    s.gyro_dps[2] = data->LSM6DSV320GyroZ_dps;
    s.baro_altitude_m = data->MS560702BA03Altitude_m;
    s.baro_pressure_hpa = data->MS560702BA03Pressure_hPA;
    s.low_g_valid = 1;
    s.high_g_valid = 1;
    s.gyro_valid = 1;
    s.baro_valid = s.baro_pressure_hpa > 0.0f;
    return s;
}

void Navigation::publish()
{
    const NavigationResult& r = estimator.result();
    data->KalmanFilterPositionX_m = r.position_m[0];
    data->KalmanFilterPositionY_m = r.position_m[1];
    data->KalmanFilterPositionZ_m = r.position_m[2];
    data->KalmanFilterVelocityX_mps = r.velocity_mps[0];
    data->KalmanFilterVelocityY_mps = r.velocity_mps[1];
    data->KalmanFilterVelocityZ_mps = r.velocity_mps[2];
    data->KalmanFilterAccelerationX_mps2 = r.acceleration_mps2[0];
    data->KalmanFilterAccelerationY_mps2 = r.acceleration_mps2[1];
    data->KalmanFilterAccelerationZ_mps2 = r.acceleration_mps2[2];
    data->quaternionW = r.quaternion[0];
    data->quaternionX = r.quaternion[1];
    data->quaternionY = r.quaternion[2];
    data->quaternionZ = r.quaternion[3];
    data->quaternionNorm = r.quaternion_norm_before_normalization;
    data->roll = r.euler_deg[0];
    data->pitch = r.euler_deg[1];
    data->yaw = r.euler_deg[2];
}

int Navigation::init()
{
    if (imu.init() < 0 || baro.init() < 0 || gps.init() < 0)
        return -1;
    estimator.reset(sample());
    publish();
    baro.startConversion();
    return 0;
}

int Navigation::update()
{
    const uint32_t now_us = micros();
    const uint32_t elapsed_us = now_us - lastLoop_us;
    lastLoop_us = now_us;
    if (elapsed_us == 0U)
        return 0;

    imu.update();
    baro.update();
    gps.update();
    estimator.step(sample(), elapsed_us);
    publish();
    baro.startConversion();
    return 0;
}

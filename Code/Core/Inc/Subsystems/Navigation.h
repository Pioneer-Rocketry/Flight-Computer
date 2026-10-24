#ifndef SRC_SUBSYSTEMS_NAVIGATION_H_
#define SRC_SUBSYSTEMS_NAVIGATION_H_

#include "Subsystem.h"
#include "Subsystems/NavigationEstimator.h"
#include "Devices/SPI_Devices/LSM6DSV320.h"
#include "Devices/SPI_Devices/MS560702BA03.h"
#include "Devices/GPS.h"
#include "main.h"
#include "utils.h"

// Hardware adapter. All navigation mathematics lives in NavigationEstimator.
class Navigation : public Subsystem {
public:
    Navigation(DataContainer* data, SPI_HandleTypeDef* spiBus,
               UART_HandleTypeDef* uart, uint8_t* gpsRxBuffer);
    int init() override;
    int update() override;

private:
    NavigationSample sample() const;
    void publish();

    LSM6DSV320 imu;
    MS560702BA03 baro;
    GPS gps;
    NavigationEstimator estimator;
    uint32_t lastLoop_us;
};

#endif

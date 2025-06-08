#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdio.h> // for snprintf

#include "data.h"
#include "I2C_Devices/I2C_Device.hpp"
#include "SPI_Devices/SPI_Device.hpp"

enum State {
    INITIALIZING        = 0,
    SYSTEM_CHECKS       = 1,
    LOCALIZING          = 2,
    READY_FOR_FLIGHT    = 3,
    ACCELERATING        = 4,
    ASCENDING           = 5,
    DESCENDING          = 6,
    UNDER_DROUGE        = 7,
    UNDER_MAIN          = 8,
    LANDED              = 9
};

#define NUM_I2C_DEVICES 1
#define NUM_SPI_DEVICES 1

#define MIN_ACCELERATION 2 * GRAVITY // 2G
#define MIN_ACCELERATION_TIME 300 // ms

#define MIN_DECCELERATION -1 * GRAVITY // -1G
#define MIN_DECCELERATION_TIME 500 // ms

class State_Machine {
private:
    State state = INITIALIZING;
    int currentState;

    uint8_t stages;
    uint8_t stagesRemaining;

    int eventStart;

    Data *data;
    I2C_Device* i2c_devices[NUM_I2C_DEVICES];
    SPI_Device* spi_devices[NUM_SPI_DEVICES];

    char state_names[10][17] = {
        "INITIALIZING",
        "SYSTEM_CHECKS",
        "LOCALIZING",
        "READY_FOR_FLIGHT",
        "ACCELERATING",
        "DECCELERATING",
        "DESCENDING",
        "UNDER_DROUGE",
        "UNDER_MAIN",
        "LANDED"
    };

    // State Switch Conditions
    void switch_to_ready_for_flight();
    void switch_to_accelerating();
    void switch_to_deccelerating();
    void switch_to_descending();
    void switch_to_under_drouge();
    void switch_to_under_main();
    void switch_to_landed();

    void switch_to_next_state();

    char buffer[64];
    void log_state();
    void clear_buffer();

public:
    State_Machine(Data *data, I2C_Device* i2c_devices[], SPI_Device* spi_devices[]);

    void system_checks();
    void localize();
    void arm();

    void start();
    State loop();
    State get_state();
};

#endif
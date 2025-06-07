#include "state_machine.h"

State_Machine::State_Machine(Data *data, I2C_Device* i2c_devices[], SPI_Device* spi_devices[]) {
    this->data = data;

    for (int i = 0; i < NUM_I2C_DEVICES; i++) this->i2c_devices[i] = i2c_devices[i];
    for (int i = 0; i < NUM_SPI_DEVICES; i++) this->spi_devices[i] = spi_devices[i];
}

State State_Machine::loop() {

    // Check to see if all I2C Devices are working
    for (int i = 0; i < NUM_I2C_DEVICES; i++) {
        i2c_devices[i]->loop();
    }

    // Check to see if all SPI Devices are working
    for (int i = 0; i < NUM_SPI_DEVICES; i++) {
        spi_devices[i]->loop();
    }

    // Run through all the the switch conditions
    switch (this->state) {
        case LOCALIZING:
            switch_to_ready_for_flight();
            break;
        case READY_FOR_FLIGHT:
            switch_to_accelerating();
            break;
        case ACCELERATING:
            switch_to_deccelerating();
            switch_to_descending();
            switch_to_landed(); // Something is VERY wrong ¯\_(ツ)_/¯
            break;
        case ASCENDING:
            switch_to_descending();
            switch_to_accelerating();
            break;
        case DESCENDING:
            switch_to_under_drouge();
            switch_to_landed();
            break;
        case UNDER_DROUGE:
            switch_to_under_main();
            switch_to_landed();
            break;
        case UNDER_MAIN:
            switch_to_landed();
            break;
        case LANDED:
            break;

        default: // Log
            break;
    }

    return this->state;
}

State State_Machine::get_state() {
    return this->state;
}

void State_Machine::start() {
    log_state();

    switch_to_next_state();
}

void State_Machine::switch_to_ready_for_flight() {
}

void State_Machine::switch_to_accelerating() {
    // Switch the state if the magnitude of the acceleration is greater than 2g, for at least .3 seconds
    if (data->rotated_LowG_Accel.magnitude() < MIN_ACCELERATION)
    {
        eventStart = 0;
        return;
    }

    if (eventStart == 0) {
        eventStart = HAL_GetTick();
        return;
    }

    if (HAL_GetTick() - eventStart > MIN_ACCELERATION_TIME) {
        eventStart = 0;
        switch_to_next_state();
    }
}

void State_Machine::switch_to_deccelerating() {
    // Switch the state if the magnitude of the acceleration is less than -1G, for at least .5 seconds
    if (data->rotated_LowG_Accel.magnitude() < MIN_DECCELERATION)
    {
        eventStart = 0;
        return;
    }

    if (eventStart == 0) {
        eventStart = HAL_GetTick();
        return;
    }

    if (HAL_GetTick() - eventStart > MIN_DECCELERATION_TIME) {
        eventStart = 0;
        switch_to_next_state();
    }

}

void State_Machine::switch_to_descending() {
}

void State_Machine::switch_to_under_drouge() {
}

void State_Machine::switch_to_under_main() {
}

void State_Machine::switch_to_landed() {
}

void State_Machine::switch_to_next_state() {
    switch (state) {
        case INITIALIZING:      state = SYSTEM_CHECKS;      break;
        case SYSTEM_CHECKS:     state = LOCALIZING;         break;
        case LOCALIZING:        state = READY_FOR_FLIGHT;   break;
        case READY_FOR_FLIGHT:  state = ACCELERATING;       break;
        case ACCELERATING:      state = ASCENDING;          break;
        case ASCENDING:         state = DESCENDING;         break;
        case DESCENDING:        state = UNDER_DROUGE;       break;
        case UNDER_DROUGE:      state = UNDER_MAIN;         break;
        case UNDER_MAIN:        state = LANDED;             break;
        case LANDED:                                        break;
        default:                state = INITIALIZING;       break;
    }

    data->state = state;
    log_state();
}

void State_Machine::system_checks() {
    if (state != SYSTEM_CHECKS) {
        clear_buffer();
        snprintf(buffer, sizeof(buffer), "Initializing %s\r\n");

        while (1) {
            // Flash failed to start
            HAL_UART_Transmit(data->uart, (uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
            HAL_Delay(1000);
        }
    }
    log_state();

    // Check to see if all I2C Devices are working
    for (int i = 0; i < NUM_I2C_DEVICES; i++) {

        clear_buffer();
        snprintf(buffer, sizeof(buffer), "Initializing %s I2C Device\r\n", i2c_devices[i]->get_name());
        HAL_UART_Transmit(data->uart, (uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);

        if (!i2c_devices[i]->begin()) {
            clear_buffer();
            int len = snprintf(buffer, sizeof(buffer), "Failed to start %s\r\n", i2c_devices[i]->get_name());

            while (1) {
                // I2C Device failed to start
                HAL_UART_Transmit(data->uart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
                HAL_Delay(1000);
            }
        }
    }

    // Check to see if all SPI Devices are working
    for (int i = 0; i < NUM_SPI_DEVICES; i++) {

        clear_buffer();
        snprintf(buffer, sizeof(buffer), "Initializing %s SPI Device\r\n", spi_devices[i]->get_name());
        HAL_UART_Transmit(data->uart, (uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);

        if (!spi_devices[i]->begin()) {
            clear_buffer();
            int len = snprintf(buffer, sizeof(buffer), "Failed to start %s\r\n", spi_devices[i]->get_name());

            while (1) {
                // SPI Device failed to start
                HAL_UART_Transmit(data->uart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
                HAL_Delay(1000);
            }
        }
    }

    switch_to_next_state();
}

void State_Machine::localize() {
    // Run kalman filter until it has an optimal position

    switch_to_next_state();
}

void State_Machine::clear_buffer() {
    for (int i=0; i < sizeof(buffer); i++) buffer[i] = 0;
}

void State_Machine::log_state() {
    clear_buffer();

    snprintf(buffer, sizeof(buffer), "Current State: %s\r\n", this->state_names[this->state]);
    HAL_UART_Transmit(data->uart, (uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
}
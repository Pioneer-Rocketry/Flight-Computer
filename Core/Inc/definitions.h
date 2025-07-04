#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/**
 * This file contains definitions of constants used in the project
 * Constant, conversion factors, settings, etc
 */

// Settings
#define MAX_RADIO_PACKET_SIZE 10
#define RADIO_FREQ 915000000 // Hz

#define NUM_I2C_DEVICES 1
#define NUM_SPI_DEVICES 2

#define MIN_ACCELERATION 2 * GRAVITY // 2G
#define MIN_ACCELERATION_TIME 300 // ms

#define MIN_DECCELERATION -1 * GRAVITY // -1G
#define MIN_DECCELERATION_TIME 500 // ms

#define LOOP_FREQ 100 // Hz
#define LOOP_TIME (1000000 / LOOP_FREQ) // us

// Conversion factors
#define RAD_TO_DEG  180/M_PI
#define DEG_TO_RAD  M_PI/180
#define FT_TO_M     0.3048

// Constants
#define GRAVITY     9.80665f

#endif /* DEFINITIONS_H */

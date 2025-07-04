#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/**
 * This file contains definitions of constants used in the project
 * Constant, conversion factors, settings, etc
 */

//
// Settings
//

// Max Number of Bytes in a Radio Packet
#define MAX_RADIO_PACKET_SIZE 10 // Bytes

// LoRa Radio Frequency
#define RADIO_FREQ 915000000 // Hz

// Number of I2C and SPI devices
#define NUM_I2C_DEVICES 1 // Barometer
#define NUM_SPI_DEVICES 2 // Imu, Radio

// Minimum amount of acceleration to switch into the launch state
#define MIN_ACCELERATION 2 * GRAVITY // 2G

// Minimum amount of time while accelerating to switch into the launch state
#define MIN_ACCELERATION_TIME 300 // ms

// Minimum amount of deceleration to switch into the landing state
#define MIN_DECCELERATION -1 * GRAVITY // -1G

// Minimum amount of time while decelerating to switch into the landing state
#define MIN_DECCELERATION_TIME 500 // ms

// Operating loop frequency
#define LOOP_FREQ 100 // Hz
#define LOOP_TIME (1000000 / LOOP_FREQ) // us

//
// Conversion factors
//

#define RAD_TO_DEG  180/M_PI
#define DEG_TO_RAD  M_PI/180
#define FT_TO_M     0.3048

//
// Constants
//

#define GRAVITY     9.80665f

#endif /* DEFINITIONS_H */

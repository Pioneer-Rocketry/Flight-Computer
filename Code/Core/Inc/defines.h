/*
 * defines.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define GRAVITY 9.81f

#define DEG_TO_RAD M_PI / 180.0f
#define RAD_TO_DEG 180.0f / M_PI

#define NAVIGATION_TARGET_FREQ 	500 // hz
#define NAVIGATION_TARGET_DT	1/NAVIGATION_TARGET_FREQ

#define KALMAN_FILTER_NUM_OF_STATES 9		// Position (X, Y, Z), Velocity (X, Y, Z), Acceleration (X, Y, Z)
#define KALMAN_FILTER_NUM_OF_MEASUREMENTS 7	// Low G Acceleration (X, Y, Z), High G Acceleration (X, Y, Z), Barometric Altitude

#endif /* INC_DEFINES_H_ */

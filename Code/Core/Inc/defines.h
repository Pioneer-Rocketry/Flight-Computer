/*
 * defines.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define GRAVITY 9.81f
#define DEG_TO_RAD M_PI / 180

#define KALMAN_FILTER_NUM_OF_STATES 6		// Position (X, Y, Z), Velocity (X, Y, Z)
#define KALMAN_FILTER_NUM_OF_MEASUREMENTS 7	// Low G Acceleration (X, Y, Z), High G Acceleration (X, Y, Z), Barometric Alt

#endif /* INC_DEFINES_H_ */

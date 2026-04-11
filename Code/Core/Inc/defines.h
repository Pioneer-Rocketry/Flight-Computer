/*
 * defines.h
 *
 *  Created on: Oct 29, 2025
 *      Author: colin
 */

#ifndef INC_DEFINES_H_
#define INC_DEFINES_H_

#define GRAVITY 9.81f

// Unit Conversions

#define DEG_TO_RAD M_PI / 180.0f
#define RAD_TO_DEG 180.0f / M_PI

#define FEET_TO_METER 0.3048f
#define METER_TO_FEET 3.28084

#define NAVIGATION_TARGET_FREQ 	500 // hz
#define NAVIGATION_TARGET_DT	1/NAVIGATION_TARGET_FREQ

#define KALMAN_FILTER_NUM_OF_STATES 3		// Altitude, Vertical Velocity, Vertical Acceleration
#define KALMAN_FILTER_NUM_OF_MEASUREMENTS 1	// Barometric Altitude

#endif /* INC_DEFINES_H_ */

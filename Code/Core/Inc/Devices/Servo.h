/*
 * Servo.h
 *
 *  Created on: Jan 08, 2026
 *      Author: colin
 */

#ifndef INC_DEVICES_SERVO_H_
#define INC_DEVICES_SERVO_H_

#include <cstdint>

#include "stm32f4xx_hal.h"

#include "DataContainer.h"

enum servo {
	SERVO_1,
	SERVO_2,
	SERVO_3,
	SERVO_4
};

class Servo {
public:

	Servo(DataContainer* data, TIM_HandleTypeDef* htim, servo servoId);

	int init();
	int update();

private:
	DataContainer* data; // Pointer to the shared DataContainer.
	TIM_HandleTypeDef* htim;
	uint32_t channel;
	servo servoId;

	int16_t minAngle;
	int16_t maxAngle;

	uint32_t minPulse;
    uint32_t maxPulse;

	int angleToPulse(int16_t angle);
};

#endif /* INC_DEVICES_SERVO_H_ */
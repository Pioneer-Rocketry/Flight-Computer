/*
 * Control.h
 *
 *  Created on: Jan 8, 2026
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_CONTROL_H_
#define SRC_SUBSYSTEMS_CONTROL_H_

#include "stm32f4xx_hal.h"

#include "Subsystem.h"
#include "DataContainer.h"

#include "Devices/Servo.h"

/**
 * @class Control
 * @brief Handles flight Control logic and sensor data fusion.
 
 */
class Control: public Subsystem {
public:
	/**
	 * @brief Control subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Control(DataContainer* data, TIM_HandleTypeDef* htim1, TIM_HandleTypeDef* htim3);

	/**
	 * @brief 
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int init() override;

	/**
	 * @brief 
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;

private:
    Servo servo1;
    Servo servo2;
    Servo servo3;
    Servo servo4;

};

#endif /* SRC_SUBSYSTEMS_CONTROL_H_ */
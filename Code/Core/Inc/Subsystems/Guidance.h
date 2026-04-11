/*
 * Guidance.h
 *
 *  Created on: April 7, 2026
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_GUIDANCE_H_
#define SRC_SUBSYSTEMS_GUIDANCE_H_

#include "stm32f4xx_hal.h"

#include "Subsystem.h"
#include "DataContainer.h"

#include "utils.h"

#define STARTING_TARGET_ANGLE 0.0f
#define TURN_ANGLE  90.0f

#define CANARD_MAX_DEFLECTION_ANGLE 10.0f
#define CANARD_OFFSET_ANGLE 10.0f

#define TURN_START  5.0f
#define TURN_END    15.0f
#define TURN_LERP   0.5f

/**
 * @class Guidance
 * @brief Handles flight Guidance logic and sensor data fusion.

 */
class Guidance: public Subsystem {
public:
	/**
	 * @brief Guidance subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Guidance(DataContainer* data, float Kp, float Ki, float Kd);

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
    float Kp;
    float Ki;
    float Kd;

    float p;
    float i;
    float d;

    float PID;
    float error;

	float lastLoop;

};

#endif /* SRC_SUBSYSTEMS_GUIDANCE_H_ */
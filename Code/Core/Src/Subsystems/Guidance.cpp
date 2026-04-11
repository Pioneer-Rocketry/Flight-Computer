/*
 * Guidance.cpp
 *
 *  Created on: April 7, 2026
 *      Author: colin
 */

#include "Subsystems/Guidance.h"

Guidance::Guidance(DataContainer* data, float Kp, float Ki, float Kd)
    : Subsystem(data)
{
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    data->p = 0.0f;
    data->i = 0.0f;
    data->d = 0.0f;

    data->error = 0.0f;
    data->PID = 0.0f;
}

int Guidance::init()
{
    return 0;
}

int Guidance::update()
{
    // If we are at the start of the turn
    if (data->flightTime > TURN_START and data->flightTime < TURN_START + TURN_LERP)
        data->target = lerp(STARTING_TARGET_ANGLE, TURN_ANGLE, (data->flightTime - TURN_START) / TURN_LERP);

        // If we are at the end of the turn
    if (data->flightTime > TURN_END and data->flightTime < TURN_END + TURN_LERP)
        data->target = lerp(TURN_ANGLE, STARTING_TARGET_ANGLE, (data->flightTime - TURN_END) / TURN_LERP);

    // If we are holding the turn
    if (data->flightTime >= TURN_START + TURN_LERP and data->flightTime <= TURN_END)
        data->target = TURN_ANGLE;

    data->error =  data->target - data->intergratedRoll;

    // dt
    if (data->flightTime > lastLoop)
		data->guidanceDt = (data->flightTime - lastLoop) / 1000.0f;
    lastLoop = data->flightTime;

    // PID calculations
    data->p  = data->error * Kp;
    data->i += (data->error * data->guidanceDt) * Ki;
    data->d  = ((data->error - data->lastError) / data->guidanceDt) * Kd;

    data->PID = data->p + data->i + data->d;
    data->PID = clamp(data->PID, -CANARD_MAX_DEFLECTION_ANGLE, CANARD_MAX_DEFLECTION_ANGLE);

    data->servo1Angle = CANARD_OFFSET_ANGLE + (int16_t)data->PID;
    data->servo2Angle = CANARD_OFFSET_ANGLE + (int16_t)data->PID;

    data->lastError = data->error;

    return 0;
}


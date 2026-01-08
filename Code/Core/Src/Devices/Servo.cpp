/*
 * Servo.cpp
 *
 *  Created on: Jan 08, 2026
 *      Author: colin
 */

#include "Devices/Servo.h"

#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

#define SERVO_MIN_PULSE   500
#define SERVO_MAX_PULSE   2500

Servo::Servo(DataContainer* data, TIM_HandleTypeDef* htim, servo servoId)
{
    this->data = data;
    this->htim = htim;
    this->servoId = servoId;

    this->minAngle = SERVO_MIN_ANGLE;
    this->maxAngle = SERVO_MAX_ANGLE;

    this->minPulse = SERVO_MIN_PULSE;
    this->maxPulse = SERVO_MAX_PULSE;

    switch (servoId) {
        case SERVO_1:
            this->channel  = TIM_CHANNEL_3;
            break;
        case SERVO_2:
            this->channel  = TIM_CHANNEL_2;
            break;
        case SERVO_3:
            this->channel  = TIM_CHANNEL_1;
            break;
        case SERVO_4:
            this->channel  = TIM_CHANNEL_4;
            break;
        
        default:
            // Handle invalid servoId if necessary
            break;
    }
}

int Servo::init()
{
    return update();
}

int Servo::update()
{
    int16_t angle;
    switch (servoId) {
        case SERVO_1:
            angle = data->servo1Angle;
            break;
        case SERVO_2:
            angle = data->servo2Angle;
            break;
        case SERVO_3:
            angle = data->servo3Angle;
            break;
        case SERVO_4:
            angle = data->servo4Angle;
            break;
        
        default:
            // Handle invalid servoId if necessary
            break;
    }

    if (angle < minAngle) angle = minAngle;
    if (angle > maxAngle) angle = maxAngle;

    __HAL_TIM_SET_COMPARE(this->htim, this->channel, angleToPulse(angle));

    return angle;
}

int Servo::angleToPulse(int16_t angle)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);

    // 20 ms period → convert to fractions
    uint32_t minPulse = arr * 500  / 20000;  // 0.5 ms
    uint32_t maxPulse = arr * 2500 / 20000;  // 2.5 ms

    return minPulse +
           (uint32_t)(angle - minAngle) *
           (maxPulse - minPulse) /
           (maxAngle - minAngle);
}
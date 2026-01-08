/*
 * Control.cpp
 *
 *  Created on: Jan 8, 2026
 *      Author: colin
 */

#include "Subsystems/Control.h"

Control::Control(DataContainer* data, TIM_HandleTypeDef* htim1, TIM_HandleTypeDef* htim3)
    : Subsystem(data),
    servo1(data, htim1, SERVO_1),
    servo2(data, htim1, SERVO_2),
    servo3(data, htim1, SERVO_3),
    servo4(data, htim3, SERVO_4)
{

}

int Control::init()
{
    servo1.init();
    servo2.init();
    servo3.init();
    servo4.init();

    return 0;
}

int Control::update()
{
    servo1.update();
    servo2.update();
    servo3.update();
    servo4.update();

    return 0;
}
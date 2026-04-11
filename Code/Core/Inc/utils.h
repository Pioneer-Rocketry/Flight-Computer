/*
 * utils.h
 *
 *  Created on: Dec 1, 2025
 *      Author: colin
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

void DWT_Init(void);
uint32_t micros(void);
void delay_us(uint32_t us);


float lerp(float a, float b, float f);
float clamp(float value, float min, float max);

#ifdef __cplusplus
}
#endif

#endif /* INC_UTILS_H_ */

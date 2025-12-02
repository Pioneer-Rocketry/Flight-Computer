/*
 * utils.c
 *
 *  Created on: Dec 1, 2025
 *      Author: colin
 */

#include "utils.h"
#include "stm32f4xx.h"

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable trace
    DWT->CYCCNT = 0;                                // Reset counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // Start counter
}

uint32_t micros(void)
{
    return DWT->CYCCNT / (SystemCoreClock / 1000000);
}

void delay_us(uint32_t us)
{
    uint32_t cycles = (SystemCoreClock / 1000000) * us;
    uint32_t start = DWT->CYCCNT;

    while ((DWT->CYCCNT - start) < cycles);
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IMU_INT1_Pin GPIO_PIN_13
#define IMU_INT1_GPIO_Port GPIOC
#define GPS_RESET_Pin GPIO_PIN_0
#define GPS_RESET_GPIO_Port GPIOC
#define GPS_INT_Pin GPIO_PIN_3
#define GPS_INT_GPIO_Port GPIOC
#define SERVO1_Pin GPIO_PIN_0
#define SERVO1_GPIO_Port GPIOA
#define SERVO2_Pin GPIO_PIN_1
#define SERVO2_GPIO_Port GPIOA
#define SPI_CS1_Pin GPIO_PIN_4
#define SPI_CS1_GPIO_Port GPIOA
#define GPS_SAFEBOOT_Pin GPIO_PIN_4
#define GPS_SAFEBOOT_GPIO_Port GPIOC
#define FLASH_CS_Pin GPIO_PIN_5
#define FLASH_CS_GPIO_Port GPIOC
#define FLASH_WP_Pin GPIO_PIN_0
#define FLASH_WP_GPIO_Port GPIOB
#define FLASH_RESET_Pin GPIO_PIN_1
#define FLASH_RESET_GPIO_Port GPIOB
#define IMU_INT2_Pin GPIO_PIN_2
#define IMU_INT2_GPIO_Port GPIOB
#define PYRO3_CONT_Pin GPIO_PIN_12
#define PYRO3_CONT_GPIO_Port GPIOB
#define LORA_DIO1_Pin GPIO_PIN_13
#define LORA_DIO1_GPIO_Port GPIOB
#define LORA_SEL_Pin GPIO_PIN_14
#define LORA_SEL_GPIO_Port GPIOB
#define PYRO3_TRIGGER_Pin GPIO_PIN_15
#define PYRO3_TRIGGER_GPIO_Port GPIOB
#define PYRO2_TRIGGER_Pin GPIO_PIN_6
#define PYRO2_TRIGGER_GPIO_Port GPIOC
#define PYRO2_CONT_Pin GPIO_PIN_7
#define PYRO2_CONT_GPIO_Port GPIOC
#define PYRO1_CONT_Pin GPIO_PIN_8
#define PYRO1_CONT_GPIO_Port GPIOC
#define PYRO_ARMED_Pin GPIO_PIN_9
#define PYRO_ARMED_GPIO_Port GPIOC
#define PYRO1_TRIGGER_Pin GPIO_PIN_8
#define PYRO1_TRIGGER_GPIO_Port GPIOA
#define LORA_BUSY_Pin GPIO_PIN_4
#define LORA_BUSY_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

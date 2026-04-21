/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include "tusb.h"
#include "dfuBootloader.h"
#include "usbHelper.h"

#include "DataContainer.h"

#include "Subsystems/Telemetry.h"
#include "Subsystems/Guidance.h"
#include "Subsystems/Navigation.h"
#include "Subsystems/Control.h"
#include "Subsystems/Logging.h"

#include "utils.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RADIO_INSTALLED

#define PYRO_ARM_THRESHOLD_VOLTAGE 3.0f
#define LAUNCH_DETECT_THRESHOLD_mps2 20.0f
#define COAST_DETECT_THRESHOLD_mps2 9.0f
#define DECENT_DETECT_THRESHOLD_MPS -20.0f
#define DROGUE_DETECT_THRESHOLD_MPS -10.0f
#define MAIN_DETECT_THRESHOLD_MPS -5.0f
#define LANDED_DETECT_THRESHOLD_mps2 2.0f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

#define USB_BUF_LEN CFG_TUD_CDC_TX_BUFSIZE

char usbTxBuffer[USB_BUF_LEN];
char usbRxBuffer[USB_BUF_LEN];

uint16_t usbTxBufferLen;
uint16_t usbRxBufferLen;

#define GPS_BUFFER_SIZE 512

uint8_t gpsRxBuffer[GPS_BUFFER_SIZE];

DataContainer data;

// Radio radio(&data, &hspi1);
Telemetry telemetry(&data, &hspi1);

Guidance guidance(&data, 0.1f, 0.0f, 0.05f);
Navigation navigation(&data, &hspi1, &huart4, gpsRxBuffer);
Control control(&data, &htim1, &htim3);
Logging logging(&data, &hspi1);

uint16_t rawAdcValue[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
extern "C" void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_UART4_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void checkArmed();
void checkLaunched();
void checkCoasting();
void checkDescent();
void checkDrogue();
void checkMain();
void checkLanded();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  dfuCheckAndJumpBootloader();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // HAL_UART_Receive_DMA(&huart4, gpsRxBuffer, GPS_BUFFER_SIZE);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawAdcValue, 4);

  tud_init(BOARD_TUD_RHPORT);

  DWT_Init();

  data.state = DataContainer::INITIALIZATION;

  #ifdef RADIO_INSTALLED

  if (telemetry.init() < 0)
  {
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Error while Initializing Telemetry!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
	  while (1)
    {
      // Send Error Message over USB CDC
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
      HAL_Delay(1000);
    }
  }

  #endif

  if (guidance.init() < 0)
  {
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Error while Initializing Guidance!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
	  while (1)
    {
      // Send Error Message over USB CDC
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
      HAL_Delay(1000);
    }
  }


  if (navigation.init() < 0)
  {
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Error while Initializing Navigation!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
	  while (1)
    {
      // Send Error Message over USB CDC
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
      HAL_Delay(1000);
    }
  }

  if (control.init() < 0)
  {
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Error while Initializing Control!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
	  while (1)
    {
      // Send Error Message over USB CDC
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
      HAL_Delay(1000);
    }
  }

  if (logging.init() < 0)
  {
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Error while Initializing Logging!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
    while (1)
    {
      // Send Error Message over USB CDC
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
      HAL_Delay(1000);
    }
  }

  usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Initialization Complete \r\n");
  cdcSendMessage(usbTxBuffer, usbTxBufferLen);

  tud_init(BOARD_TUD_RHPORT);

  /* USER CODE END 2 */

  uint32_t lastPrint = HAL_GetTick();

  data.launchTime = HAL_GetTick() / 1000.0f;

  data.state = DataContainer::PRELAUNCH;

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    tud_task();

    data.flightTime = (HAL_GetTick() / 1000.0f) - data.launchTime;

	  navigation.update();
    control.update();

    #ifdef RADIO_INSTALLED
    telemetry.update();
    #endif

    switch (data.state)
    {
      case DataContainer::PRELAUNCH:
        checkArmed();
        checkLaunched();

        data.servo1Angle = 10;
        data.servo2Angle = 10;
        data.servo3Angle = 10;
        data.servo4Angle = 10;
        break;

      case DataContainer::ARMED:
        checkLaunched();

        guidance.update();
        break;

      case DataContainer::LAUNCH:
        checkCoasting();

        guidance.update();
        break;

      case DataContainer::COAST:
        checkDescent();

        guidance.update();
        break;

      case DataContainer::DESCENT:
        checkDrogue();
        break;

      case DataContainer::DROGUE:
        checkMain();
        break;

      case DataContainer::MAIN:
        checkLanded();
        break;

      case DataContainer::LANDED:
        break;

      default:
        break;
    }

    logging.update();

    if (HAL_GetTick() - lastPrint >= 500)
    {
      lastPrint = HAL_GetTick();
      usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN,
        "\r\n=== Time : %lu ms T+ %.2f State: %d ===\r\n"
        "IMU: Acc Low G (%.2f, %.2f, %.2f) m/s² | Acc High G (%.2f, %.2f, %.2f) m/s² | Gyro (%.2f, %.2f, %.2f) dps\r\n"
        "Baro: Temp %.2f °C | Pressure %.2f hPA | Altitude %.2f m\r\n"
        "GPS: Lat %.6f | Lon %.6f | Alt %.2f m | Fix %d | Sats %d | UTC %s\r\n"
        "PID: P %.2f | I %.2f | D %.2f | PID %.2f | DT: %.7f\r\n"
        "Roll: %.2f | Target: %.2f | Error: %.2f \r\n"
        "Pyro Voltages: Arm %.2f V | Pyro1 %.2f V | Pyro2 %.2f V | Pyro3 %.2f V\r\n"
        "Vertical: Acc %.2f m/s² | Vel %.2f m/s | Alt %.2f m\r\n"
        "Servo Angles: %d, %d\r\n",
        HAL_GetTick(), data.flightTime, data.state,
        data.LSM6DSV320LowGAccelX_mps2, data.LSM6DSV320LowGAccelY_mps2, data.LSM6DSV320LowGAccelZ_mps2,
        data.LSM6DSV320HighGAccelX_mps2, data.LSM6DSV320HighGAccelY_mps2, data.LSM6DSV320HighGAccelZ_mps2,
        data.LSM6DSV320GyroX_dps, data.LSM6DSV320GyroY_dps, data.LSM6DSV320GyroZ_dps,
        data.MS560702BA03Temperature_C, data.MS560702BA03Pressure_hPA, data.MS560702BA03Altitude_m,
        data.GPSLatitude, data.GPSLongitude, data.GPSAltitude_m, data.GPSFix, data.GPSNumSatellites, data.GPSUTCTime,
        data.p, data.i, data.d, data.PID, data.guidanceDt,
        data.roll, data.target, data.error,
        data.pyroArmVoltage, data.pyro1Voltage, data.pyro2Voltage, data.pyro3Voltage,
        data.verticalAcceleration_mps2, data.intergratedVerticalVelocity_mps, data.intergratedVerticalAltitude_m,
        data.servo1Angle, data.servo2Angle
      );
      cdcSendMessage(usbTxBuffer, usbTxBufferLen);
    }

    /* Basic Command */
    /**
     * d - dump flash
     * e - erase flash
     * h - help
     */

    if (tud_cdc_connected() && tud_cdc_available())
    {
      usbRxBufferLen = tud_cdc_read(usbRxBuffer, USB_BUF_LEN);
      for (uint16_t i = 0; i < usbRxBufferLen; i++)
      {
        char c = usbRxBuffer[i];
        switch (c)
        {
          case 'd':
            logging.dumpFlash();
            break;
          case 'e':
            // Send "Are you sure? (y/n)" prompt
            usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Are you sure? (y/n)\r\n");
            cdcSendMessage(usbTxBuffer, usbTxBufferLen);

            // Wait for response
            while (!tud_cdc_available());

            // Read response
            usbRxBufferLen = tud_cdc_read(usbRxBuffer, USB_BUF_LEN);
            if (usbRxBufferLen > 0 && (usbRxBuffer[0] == 'y' || usbRxBuffer[0] == 'Y'))
            {
              logging.erase();
              usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Flash erased.\r\n");
              cdcSendMessage(usbTxBuffer, usbTxBufferLen);
            }

            break;
          case 'h':
            usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Commands:\r\n d - dump flash\r\n e - erase flash\r\n h - help\r\n s - status\r\n r - reboot\r\n");
            cdcSendMessage(usbTxBuffer, usbTxBufferLen);
            break;
          default:
            break;
        }
      }
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 27;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = SERVO3_Pin|SERVO2_Pin|SERVO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE END TIM1_Init 2 */
  // HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 27;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA2_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LORA_DIO0_Pin|PYRO3_TRIGGER_Pin|PRYO2_TRIGGER_Pin
                          |PRYO1_TRIGGER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SPI_CS1_Pin|LORA_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LORA_CS_Pin|FLASH_RESET_Pin|FLASH_WP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BARO_CS_Pin|FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SPI_CS1_Pin LORA_DIO0_Pin PYRO3_TRIGGER_Pin PRYO2_TRIGGER_Pin
                           PRYO1_TRIGGER_Pin LORA_RESET_Pin */
  GPIO_InitStruct.Pin = SPI_CS1_Pin|LORA_DIO0_Pin|PYRO3_TRIGGER_Pin|PRYO2_TRIGGER_Pin
                          |PRYO1_TRIGGER_Pin|LORA_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_INT1_Pin */
  GPIO_InitStruct.Pin = IMU_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_CS_Pin */
  GPIO_InitStruct.Pin = IMU_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IMU_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_INT2_Pin */
  GPIO_InitStruct.Pin = IMU_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LORA_CS_Pin BARO_CS_Pin FLASH_CS_Pin FLASH_RESET_Pin
                           FLASH_WP_Pin */
  GPIO_InitStruct.Pin = LORA_CS_Pin|BARO_CS_Pin|FLASH_CS_Pin|FLASH_RESET_Pin
                          |FLASH_WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : GPS_INT_Pin */
  GPIO_InitStruct.Pin = GPS_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPS_INT_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if (hadc->Instance == ADC1) {
    data.pyro2Voltage   = (rawAdcValue[0] / 4095.0f) * 3.3f * 2.7f;
    data.pyro1Voltage   = (rawAdcValue[1] / 4095.0f) * 3.3f * 2.7f;
    data.pyroArmVoltage = (rawAdcValue[2] / 4095.0f) * 3.3f * 11.0f;
    data.pyro3Voltage   = (rawAdcValue[3] / 4095.0f) * 3.3f * 2.7f;

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)rawAdcValue, 4);
  }
}

void tud_dfu_runtime_reboot_to_dfu_cb(void)
{
  char reboot_msg[] = "Rebooting into DFU mode...\r\n";
  cdcSendMessage(reboot_msg, strlen(reboot_msg));

  // Ensure message is sent
  for (int i = 0; i < 10; i++) {
    tud_task();
    HAL_Delay(5);
  }

  // Request bootloader entry (never returns)
  dfuRequestBootloaderEntry();

}

void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, uint8_t const* data, uint16_t length)
{
  (void)alt;
  (void)block_num;
  (void)data;
  (void)length;

  // This callback is called during the download process
  // We don't need to do anything here since we're using runtime DFU
}

void tud_dfu_manifest_cb(uint8_t alt)
{
  (void)alt;

  // DFU upload complete, reboot back to application
  // Give a short delay to ensure USB transaction completes
  HAL_Delay(100);

  // System reset to return to application
  NVIC_SystemReset();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

void checkArmed()
{
  if (data.pyroArmVoltage >= PYRO_ARM_THRESHOLD_VOLTAGE)
  {
    data.state = DataContainer::ARMED;

    // Reset state variables
    data.verticalAcceleration_mps2 = 0.0f;
    data.intergratedVerticalVelocity_mps = 0.0f;
    data.intergratedVerticalAltitude_m = 0.0f;
    data.intergratedRoll = 0.0f;

    data.lastError = 0.0f;

    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Rocket Armed!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }
}

void checkLaunched()
{
  if (data.verticalAcceleration_mps2 > LAUNCH_DETECT_THRESHOLD_mps2)
  {
    data.state = DataContainer::LAUNCH;

    // Reset state variables
    data.verticalAcceleration_mps2 = 0.0f;
    data.intergratedVerticalVelocity_mps = 0.0f;
    data.intergratedVerticalAltitude_m = 0.0f;
    data.intergratedRoll = 0.0f;

    data.lastError = 0.0f;

    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Launch Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }
}

void checkCoasting()
{
  if (data.verticalAcceleration_mps2 < COAST_DETECT_THRESHOLD_mps2)
  {
    data.state = DataContainer::COAST;
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Coasting Phase Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }
}

void checkDescent()
{
  if (data.intergratedVerticalVelocity_mps > DECENT_DETECT_THRESHOLD_MPS)
  {
    data.state = DataContainer::DESCENT;
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Descent Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }
}

void checkDrogue()
{
  if (data.intergratedVerticalVelocity_mps > DROGUE_DETECT_THRESHOLD_MPS)
  {
    data.state = DataContainer::DROGUE;
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Drogue Deployment Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }

}

void checkMain()
{
  if (data.intergratedVerticalVelocity_mps > MAIN_DETECT_THRESHOLD_MPS)
  {
    data.state = DataContainer::MAIN;
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Main Deployment Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }
}

void checkLanded()
{
  if (data.verticalAcceleration_mps2 > GRAVITY + LANDED_DETECT_THRESHOLD_mps2 &&
      data.verticalAcceleration_mps2 < GRAVITY - LANDED_DETECT_THRESHOLD_mps2)
  {
    data.state = DataContainer::LANDED;
    usbTxBufferLen = snprintf((char*)usbTxBuffer, USB_BUF_LEN, "Landing Detected!\r\n");
    cdcSendMessage(usbTxBuffer, usbTxBufferLen);
  }

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

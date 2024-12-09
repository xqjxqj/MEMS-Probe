/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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
#define MAG_X_DOUT_Pin GPIO_PIN_2
#define MAG_X_DOUT_GPIO_Port GPIOE
#define MAG_X_DIN_Pin GPIO_PIN_3
#define MAG_X_DIN_GPIO_Port GPIOE
#define MAG_Y_DOUT_Pin GPIO_PIN_4
#define MAG_Y_DOUT_GPIO_Port GPIOE
#define MAG_Y_DIN_Pin GPIO_PIN_5
#define MAG_Y_DIN_GPIO_Port GPIOE
#define ADC1_CS_Pin GPIO_PIN_0
#define ADC1_CS_GPIO_Port GPIOC
#define ADC2_CS_Pin GPIO_PIN_1
#define ADC2_CS_GPIO_Port GPIOC
#define ADC3_CS_Pin GPIO_PIN_2
#define ADC3_CS_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_3
#define SW1_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_4
#define SW2_GPIO_Port GPIOA
#define FLOW_CPU_Pin GPIO_PIN_0
#define FLOW_CPU_GPIO_Port GPIOB
#define GR_CPU_Pin GPIO_PIN_1
#define GR_CPU_GPIO_Port GPIOB
#define PULSE_CPU_Pin GPIO_PIN_2
#define PULSE_CPU_GPIO_Port GPIOB
#define SENS_Power_Pin GPIO_PIN_10
#define SENS_Power_GPIO_Port GPIOC
#define Mag_Power_Pin GPIO_PIN_11
#define Mag_Power_GPIO_Port GPIOC
#define SHK_Power_Pin GPIO_PIN_12
#define SHK_Power_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_2
#define LED_R_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_3
#define LED_G_GPIO_Port GPIOD
#define MAG_Z_DIN_Pin GPIO_PIN_0
#define MAG_Z_DIN_GPIO_Port GPIOE
#define MAG_Z_DOUT_Pin GPIO_PIN_1
#define MAG_Z_DOUT_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

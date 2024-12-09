/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os2.h"
#include "rl_usb.h"
#include "rl_fs.h"
#include "EventRecorder.h"

#include "stdio.h"
#include "string.h"

#include "user_usart.h"
#include "UserApp.h"
#include "ringbuffer.h"
#include "CMDTask.h"
#include "UserApp.h"
#include "user_rtc.h"
#include "UserStorage.h"
#include "DataProcess.h"
#include "WellLineSensor.h"

#include "ADS8345.h"
#include "AD8557.h"

#include "qbus.h"
#include "mwd_interface.h"
  // #include "fsmc_nand.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
  // extern TIM_HandleTypeDef htim1;
  // extern TIM_HandleTypeDef htim3;
  // extern TIM_HandleTypeDef htim8;
  // extern SPI_HandleTypeDef hspi2;
  extern ADC_HandleTypeDef hadc1;
  extern ADC_HandleTypeDef hadc2;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
  
 extern uint8_t U_ID[12];
  extern  volatile uint8_t  Power_off;

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC5SampleRate 4
#define TIM3CLK SystemCoreClock
#define TIM1PSC 1000
#define TIM3PSC 1000
#define TIM1CLK SystemCoreClock
#define ExtADCSampleRate 50
#define Shk_Pin GPIO_PIN_4
#define Shk_GPIO_Port GPIOE
#define MagV_Pin GPIO_PIN_5
#define MagV_GPIO_Port GPIOE
#define SenV_Pin GPIO_PIN_6
#define SenV_GPIO_Port GPIOE
#define ARM_RESET_Pin GPIO_PIN_10
#define ARM_RESET_GPIO_Port GPIOG
#define ADC1_CS_Pin GPIO_PIN_0
#define ADC1_CS_GPIO_Port GPIOC
#define ADC2_CS_Pin GPIO_PIN_1
#define ADC2_CS_GPIO_Port GPIOC
#define ADC3_CS_Pin GPIO_PIN_2
#define ADC3_CS_GPIO_Port GPIOC
#define BAT1SW_Pin GPIO_PIN_3
#define BAT1SW_GPIO_Port GPIOA
#define BAT2SW_Pin GPIO_PIN_4
#define BAT2SW_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define FLOW_Pin GPIO_PIN_0
#define FLOW_GPIO_Port GPIOB
#define GR_Pin GPIO_PIN_1
#define GR_GPIO_Port GPIOB
#define PULSE_Pin GPIO_PIN_2
#define PULSE_GPIO_Port GPIOB
#define bUSBPlug_Pin GPIO_PIN_10
#define bUSBPlug_GPIO_Port GPIOA
#define MagGainX_Pin GPIO_PIN_10
#define MagGainX_GPIO_Port GPIOC
#define MagGainY_Pin GPIO_PIN_12
#define MagGainY_GPIO_Port GPIOC
#define MagGainZ_Pin GPIO_PIN_3
#define MagGainZ_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_5
#define LED_R_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_6
#define LED_G_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define GPIO_PIN_0_H16 ((uint32_t)0x00010000)  /* Pin 0 selected    */
#define GPIO_PIN_1_H16 ((uint32_t)0x00020000)  /* Pin 1 selected    */
#define GPIO_PIN_2_H16 ((uint32_t)0x00040000)  /* Pin 2 selected    */
#define GPIO_PIN_3_H16 ((uint32_t)0x00080000)  /* Pin 3 selected    */
#define GPIO_PIN_4_H16 ((uint32_t)0x00100000)  /* Pin 4 selected    */
#define GPIO_PIN_5_H16 ((uint32_t)0x00200000)  /* Pin 5 selected    */
#define GPIO_PIN_6_H16 ((uint32_t)0x00400000)  /* Pin 6 selected    */
#define GPIO_PIN_7_H16 ((uint32_t)0x00800000)  /* Pin 7 selected    */
#define GPIO_PIN_8_H16 ((uint32_t)0x01000000)  /* Pin 8 selected    */
#define GPIO_PIN_9_H16 ((uint32_t)0x02000000)  /* Pin 9 selected    */
#define GPIO_PIN_10_H16 ((uint32_t)0x04000000) /* Pin 10 selected   */
#define GPIO_PIN_11_H16 ((uint32_t)0x08000000) /* Pin 11 selected   */
#define GPIO_PIN_12_H16 ((uint32_t)0x10000000) /* Pin 12 selected   */
#define GPIO_PIN_13_H16 ((uint32_t)0x20000000) /* Pin 13 selected   */
#define GPIO_PIN_14_H16 ((uint32_t)0x40000000) /* Pin 14 selected   */
#define GPIO_PIN_15_H16 ((uint32_t)0x80000000) /* Pin 15 selected   */

#define LED_R_Pin_H16 GPIO_PIN_5_H16
#define LED_G_Pin_H16 GPIO_PIN_6_H16

#define LED_R_ON LED_R_GPIO_Port->BSRR = (uint32_t)LED_R_Pin_H16 // L
#define LED_R_OFF LED_R_GPIO_Port->BSRR = (uint32_t)LED_R_Pin    // H
  // #define LED_R_FLASH HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin)

#define LED_R_FLASH LED_R_GPIO_Port->BSRR = ((LED_R_GPIO_Port->ODR & LED_R_Pin) << 16) | (~(LED_R_GPIO_Port->ODR) & LED_R_Pin)

#define LED_G_ON LED_G_GPIO_Port->BSRR = (uint32_t)LED_G_Pin_H16 // L
#define LED_G_OFF LED_G_GPIO_Port->BSRR = (uint32_t)LED_G_Pin    // H
// #define LED_G_FLASH HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin)
#define LED_G_FLASH LED_G_GPIO_Port->BSRR = ((LED_G_GPIO_Port->ODR & LED_G_Pin) << 16) | (~(LED_G_GPIO_Port->ODR) & LED_G_Pin)

#define ADC1_CS_Pin_H16 GPIO_PIN_0_H16
#define ADC2_CS_Pin_H16 GPIO_PIN_1_H16
#define ADC3_CS_Pin_H16 GPIO_PIN_2_H16
#define ADC1_CS_L ADC1_CS_GPIO_Port->BSRR = (uint32_t)ADC1_CS_Pin_H16 // L
#define ADC1_CS_H ADC1_CS_GPIO_Port->BSRR = (uint32_t)ADC1_CS_Pin     // H
#define ADC2_CS_L ADC2_CS_GPIO_Port->BSRR = (uint32_t)ADC2_CS_Pin_H16 // L
#define ADC2_CS_H ADC2_CS_GPIO_Port->BSRR = (uint32_t)ADC2_CS_Pin     // H
#define ADC3_CS_L ADC3_CS_GPIO_Port->BSRR = (uint32_t)ADC3_CS_Pin_H16 // L
#define ADC3_CS_H ADC3_CS_GPIO_Port->BSRR = (uint32_t)ADC3_CS_Pin     // H

#define SPI1_SCK_Pin_H16 GPIO_PIN_5_H16
#define SPI1_SCK_L SPI1_SCK_GPIO_Port->BSRR = (uint32_t)SPI1_SCK_Pin_H16 // L
#define SPI1_SCK_H SPI1_SCK_GPIO_Port->BSRR = (uint32_t)SPI1_SCK_Pin     // H

#define SPI1_MOSI_Pin_H16 GPIO_PIN_7_H16
#define SPI1_MOSI_L SPI1_MOSI_GPIO_Port->BSRR = (uint32_t)SPI1_MOSI_Pin_H16 // L
#define SPI1_MOSI_H SPI1_MOSI_GPIO_Port->BSRR = (uint32_t)SPI1_MOSI_Pin     // H

#define bSPI1_MISO (SPI1_MISO_GPIO_Port->IDR & SPI1_MISO_Pin)

#define BAT1SW_Pin_H16 GPIO_PIN_3_H16
#define BAT1SW_OFF BAT1SW_GPIO_Port->BSRR = (uint32_t)BAT1SW_Pin_H16 // L
#define BAT1SW_ON BAT1SW_GPIO_Port->BSRR = (uint32_t)BAT1SW_Pin      // H

#define BAT2SW_Pin_H16 GPIO_PIN_4_H16
#define BAT2SW_OFF BAT2SW_GPIO_Port->BSRR = (uint32_t)BAT2SW_Pin_H16 // L
#define BAT2SW_ON BAT2SW_GPIO_Port->BSRR = (uint32_t)BAT2SW_Pin      // H

#define PULSE_Pin_H16 GPIO_PIN_2_H16
#define PULSE_L PULSE_GPIO_Port->BSRR = (uint32_t)PULSE_Pin_H16 // L
#define PULSE_H PULSE_GPIO_Port->BSRR = (uint32_t)PULSE_Pin     // H

#define Shk_Pin_H16 GPIO_PIN_4_H16
#define Shk_OFF Shk_GPIO_Port->BSRR = (uint32_t)Shk_Pin_H16 // L
#define Shk_ON Shk_GPIO_Port->BSRR = (uint32_t)Shk_Pin      // H

#define MagV_Pin_H16 GPIO_PIN_5_H16
#define MagV_OFF MagV_GPIO_Port->BSRR = (uint32_t)MagV_Pin_H16 // L
#define MagV_ON MagV_GPIO_Port->BSRR = (uint32_t)MagV_Pin      // H

#define SenV_Pin_H16 GPIO_PIN_6_H16
#define SenV_OFF SenV_GPIO_Port->BSRR = (uint32_t)SenV_Pin_H16 // L
#define SenV_ON SenV_GPIO_Port->BSRR = (uint32_t)SenV_Pin      // H

#define MagGainX_Pin_H16 GPIO_PIN_10_H16
#define MagGainY_Pin_H16 GPIO_PIN_12_H16
#define MagGainZ_Pin_H16 GPIO_PIN_3_H16

#define MagGainX_L MagGainX_GPIO_Port->BSRR = (uint32_t)MagGainX_Pin_H16 // L
#define MagGainX_H MagGainX_GPIO_Port->BSRR = (uint32_t)MagGainX_Pin     // H

#define MagGainY_L MagGainY_GPIO_Port->BSRR = (uint32_t)MagGainY_Pin_H16 // L
#define MagGainY_H MagGainY_GPIO_Port->BSRR = (uint32_t)MagGainY_Pin     // H

#define MagGainZ_L MagGainZ_GPIO_Port->BSRR = (uint32_t)MagGainZ_Pin_H16 // L
#define MagGainZ_H MagGainZ_GPIO_Port->BSRR = (uint32_t)MagGainZ_Pin     // H

#define FLASH_RnB_Port GPIOD
#define FLASH_RnB_Pin GPIO_PIN_6

//---------------- usb ----------------
#define bUSBPlug (bUSBPlug_GPIO_Port->IDR & bUSBPlug_Pin)

  typedef enum
  {
    RAM_DMA_Sign_Empty = 0x00,
    RAM_DMA_Sign_HalfFull,
    RAM_DMA_Sign_Full,
    RAM_DMA_Sign_Undefine
  } RAM_DMA_Full_Typedef;

//--------------------- Flash ------------------------
#define Flash_RawDataConstStartA 0x55
#define Flash_RawDataConstStartB 0x55
#define Flash_RawDataConstStartC 0x55
#define Flash_RawDataConstStartD 0x55

#define Flash_RawDataConstStopA 0xaa
#define Flash_RawDataConstStopB 0xaa
// end--------------------- Flash ------------------------

//---------------------- RAM arrangement ----------------------
#define ExtADC_Chs 8
#define ExtADC_RecordLength_Each_Buffer (ExtADCSampleRate / 2)
#define ExtADC_Bytes 3

// store 24b raw adc
// RAM for ADC DMA receive buffer
// 8channels,each 3 bytes :  24bytes / record
// sample rate 200Hz, single buffer stores 100 records
#define RawADC_24b_Buffer_HalfSize (ExtADC_Chs * ExtADC_Bytes * ExtADC_RecordLength_Each_Buffer) // 128*8*3=3072
#define RawADC_24b_Buffer_A (0x20018000)
#define RawADC_24b_Buffer_B (RawADC_24b_Buffer_A + RawADC_24b_Buffer_HalfSize)

// store filtered adc
#define Filtered_ExtADC_Bytes 2
// RAM for filtered ADC receive buffer
// 8 channels, each 2 bytes :  16bytes / record
// sample rate 200Hz, single buffer stores 100 records
#define FilteredADC_Buffer_HalfSize (ExtADC_Chs * Filtered_ExtADC_Bytes * ExtADC_RecordLength_Each_Buffer) // 128*8*2=2048
#define FilteredADC_Buffer_A (RawADC_24b_Buffer_B + RawADC_24b_Buffer_HalfSize)
#define FilteredADC_Buffer_B (FilteredADC_Buffer_A + FilteredADC_Buffer_HalfSize)
#define FilteredADC_Buffer_End (FilteredADC_Buffer_B + FilteredADC_Buffer_HalfSize)

// store 16b raw adc
#define RawADC_16b_Buffer_HalfSize (ExtADC_Chs * Filtered_ExtADC_Bytes * ExtADC_RecordLength_Each_Buffer) // 128*8*2=2048
#define RawADC_16b_Buffer_A (FilteredADC_Buffer_B + FilteredADC_Buffer_HalfSize)
#define RawADC_16b_Buffer_B (RawADC_16b_Buffer_A + RawADC_16b_Buffer_HalfSize)
#define RawADC_16b_Buffer_End (RawADC_16b_Buffer_B + RawADC_16b_Buffer_HalfSize)

#define IntADC_Chs 3
#define IntADC_RecordLength_Each_Buffer (ExtADCSampleRate / 2)
#define IntADC_Bytes 2

#define IntADC_16b_Buffer_HalfSize (IntADC_Chs * IntADC_Bytes * IntADC_RecordLength_Each_Buffer) // 128*3*2=768
#define IntADC_16b_Buffer_A (RawADC_16b_Buffer_B + RawADC_16b_Buffer_HalfSize)
#define IntADC_16b_Buffer_B (IntADC_16b_Buffer_A + IntADC_16b_Buffer_HalfSize)
#define IntADC_16b_Buffer_End (IntADC_16b_Buffer_B + IntADC_16b_Buffer_HalfSize)

#define IntADC_VT_Chs 5

// board no
#define Board_SN 230601
  // #define Board_SN  230602
  // #define Board_SN  230603
  // #define Board_SN  230604

  typedef enum
  {
    QFlex200_Mag614_Probe_V01 = 0x10, // 使用QFlex185石英加�?�度+Mag614磁�?�门  WL615
    QFlex185_IGGMag_Probe_V01 = 0x20, // 使用QFlex185石英加�?�度+IGG自研磁�?�门  WL616
    MEMSs_HMC102x_Probe_V01 = 0x30,   // 使用多组MEMS加�?�度+HMC1021/2磁阻     WL618
    MEMSs_RM3100_Probe_V01 = 0x40,    // 使用单组组MEMS加�?�度+RM3100�??     WL315
  } MyID_TypeDef;

#define SN_Version MEMSs_HMC102x_Probe_V01 // 类型 WL618
#define HW_Version 0x01                      // 硬件版本
#define FW_Version 0x01                      // 固件版本

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

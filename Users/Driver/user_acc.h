/**
 * @file Untitled-1
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2021-06-08
 * @copyright Copyright (c) 2021  CAS 
 */
#ifndef __USER_ACC_H_
#define __USER_ACC_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Public define --------------------------------------------------------—----*/


/* Typedef ----------------------------------------------------------————————-*/
 
typedef enum
{
    SPI_CH_1_4=0x00,
    SPI_CH_5_6,
    SPI_CH_7_8   
} SPI_CH_Type;

/* Write data mode */
typedef enum {
   SPI_WRITE_ONE_REG = 1,         /* Write 1 ACC register */
   SPI_WRITE_TWO_REG             /* Write 2 ACC register */
   
} enWriteData;

typedef enum {
   SPI_READ_ONE_REG = 1,            /* Read one ACC register */
   SPI_READ_TWO_REG,                /* Read two ACC registers */
   SPI_READ_THREE_REG,              /* Read X,Y,Z ACC registers */

} enRegsNum;

typedef struct
{
	volatile float fAcc_X;
	volatile float fAcc_Y;
	volatile float fAcc_Z;	 
	volatile float fTemperature;	 
}ADXL355_TypeDef;


/*******************************************************************************
**************************** Internal definitions ******************************
********************************************************************************/

/* Accelerometer write command */
#define ADXL355_WRITE         0x0

/* Accelerometer read command */
#define ADXL355_READ          0x1

/* ADXL355 registers addresses */
#define DEVID_AD                 0x00
#define DEVID_MST                0x01
#define PARTID                   0x02
#define REVID                    0x03
#define STATUS                   0x04
#define FIFO_ENTRIES             0x05
#define TEMP2                    0x06
#define TEMP1                    0x07
#define XDATA3                   0x08
#define XDATA2                   0x09
#define XDATA1                   0x0A
#define YDATA3                   0x0B
#define YDATA2                   0x0C
#define YDATA1                   0x0D
#define ZDATA3                   0x0E
#define ZDATA2                   0x0F
#define ZDATA1                   0x10
#define FIFO_DATA                0x11
#define OFFSET_X_H               0x1E
#define OFFSET_X_L               0x1F
#define OFFSET_Y_H               0x20
#define OFFSET_Y_L               0x21
#define OFFSET_Z_H               0x22
#define OFFSET_Z_L               0x23
#define ACT_EN                   0x24
#define ACT_THRESH_H             0x25
#define ACT_THRESH_L             0x26
#define ACT_COUNT                0x27
#define FILTER                   0x28
#define FIFO_SAMPLES             0x29
#define INT_MAP                  0x2A
#define SYNC                     0x2B
#define RANGE                    0x2C
#define POWER_CTL                0x2D
#define SELF_TEST                0x2E
#define RESET                    0x2F

/**************************** Configuration parameters **********************/

/* Temperature parameters */
#define ADXL355_TEMP_Normal       (float)25.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_BIAS       (float)1852.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /* Accelerometer temperature change from datasheet (LSB/degC) */

/* Accelerometer parameters */
#define ADXL_RANGE     2     /* ADXL362 sensitivity: 2, 4, 8 [g] */

#define ACT_VALUE          50     /* Activity threshold value */

#define INACT_VALUE        50     /* Inactivity threshold value */

#define ACT_TIMER          100    /* Activity timer value in ms */

#define INACT_TIMER        10     /* Inactivity timer value in seconds */

#define Axis_Sensitivity_2g   256000
#define Axis_Sensitivity_4g   128000
#define Axis_Sensitivity_8g    64000

#define Self_Test_Const_Value_X 0.3f
#define Self_Test_Const_Value_Y 0.3f
#define Self_Test_Const_Value_Z 1.5f




/* Public macro -------------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/


/* Public functions ---------------------------------------------------------*/

extern uint8_t Accelerator_Init(void);
extern void ADXL355_Start_Sensor(void);
extern void ADXL355_Data_Scan(void);



#endif	 // __USER_ACC_H_ 

/*********** (C) COPYRIGHT 2021 skyhigh *****END OF FILE****/

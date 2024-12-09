/**
 * @file Untitled-1
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2021-06-26
 * @copyright Copyright (c) 2021  CAS 
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc4;

/* Private variables ---------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  ADC缓冲区存满后处理,双缓冲区中的 M0 存满
 * @param  hadc  hdc指针
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
//        RealtimeProcessRunning_Status.bBufferFull = 1;
//        RealtimeProcessRunning_Status.StoringBufferIndex = 0;
    }
    else if (hadc == &hadc2)
    {
//        Calc_Batt_Capacity((uint16_t *)(Battery_Voltage_Data_Buffer_A));
		EventRecord2 (0x02, 0x01, 0x00);
    }
}

/**
 * @brief  ADC缓冲区存满后处理,双缓冲区中的 M1存满
 * @param  hadc    hdc指针
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
//        RealtimeProcessRunning_Status.bBufferFull = 1;
//        RealtimeProcessRunning_Status.StoringBufferIndex = 1;
    }
    else if (hadc == &hadc2)
    {
         EventRecord2 (0x02, 0x02, 0x00);
    }
}


/* Public functions ---------------------------------------------------------*/







/*********** (C) COPYRIGHT 2021  skyhigh  *****END OF FILE****/

/**
 * @file DataProcess
 * @brief 数据处理
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-04-27
 * @copyright Copyright (c) 2022  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "IIR_filter.h"
/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
RAM_DMA_Full_Typedef ExtADC_Full = RAM_DMA_Sign_Empty; // 1 = halffull  2 = all full
RAM_DMA_Full_Typedef IntADC_Full = RAM_DMA_Sign_Empty; // 1 = halffull  2 = all full

uint16_t LogicPageSize = 4096;
uint8_t MixSensorDataWriteBuffer[8192]; // 4096=1page of nandflash
uint16_t MixSensorDataWriteOffset = 0;

//uint16_t Realtime_RawData[ExtADC_Chs + IntADC_VT_Chs]; // 存一组实时数据
//uint8_t bNew_Realtime_RawData = 0;

// 每4096字节作为一个最小逻辑块

// 存满2个逻辑块 写入一次存储器
uint32_t PageIndex = 0; //

uint16_t GyroTemp = 0;

uint8_t *pU8ExtADCRaw; // 写入FLASH时，外部ADC数据地址
uint8_t *pU8IntADCRaw; // 写入FLASH时，内ADC数据地址

//uint16_t *pU16ExtADCRaw; // 实时数据，外部ADC数据地址
uint16_t *pU16IntADCRaw; // 实时数据，内部ADC数据地址

#define Raw_New_Clear 0x00 // 无数据
#define Raw_New_ExtADC_Half 0x01
#define Raw_New_ExtADC_Full 0x02
#define Raw_New_IntADC_Half 0x04
#define Raw_New_IntADC_Full 0x08

uint8_t bADCRawNew = 0; // bit0=1,外部ADC-half更新了
                        // bit1=1,外部ADC-full更新了
                        // bit2=1,内部ADC-half更新了
                        // bit3=1,内部ADC-full更新了

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
        IntADC_Full = RAM_DMA_Sign_HalfFull;

        LED_R_FLASH;
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
        IntADC_Full = RAM_DMA_Sign_Full;

        LED_R_FLASH;
    }
}



/**
 * @brief 对每个page的起始16字节赋值
 *
 */
void Init_Page_IndexData(void)
{
    // 4096=1page of nandflash
    // - pageindex  4
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(PageIndex);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(PageIndex >> 8);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(PageIndex >> 16);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(PageIndex >> 24);

    PageIndex++;

    // - rtc  4
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(RTC_STAMP);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(RTC_STAMP >> 8);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(RTC_STAMP >> 16);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(RTC_STAMP >> 24);

    // - 温度 2
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(GyroTemp);
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = (uint8_t)(GyroTemp >> 8);    

    // - BAT1 2  暂时没有采集
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;

    // - BAT2 2  暂时没有采集
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;

    // - RES 2
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;
    MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = 0x00;
}


/* Public functions ---------------------------------------------------------*/

void ADC_RawData_Process(void)
{
    uint32_t i, j;

    // ------------ ExtADC ------------
    // pU8ExtADCRaw = NULL;

    if (ExtADC_Full == RAM_DMA_Sign_HalfFull)
    {
        // EventStartA(0);
        // EventStopB(0);

        ExtADC_Full = RAM_DMA_Sign_Empty;

        // 原始数据，24bit --> 16bit
        RawData_24b_16b((uint8_t *)(RawADC_24b_Buffer_A),
                        (int16_t *)(RawADC_16b_Buffer_A),
                        ExtADC_Chs,
                        ExtADC_RecordLength_Each_Buffer);

        pU8ExtADCRaw = (uint8_t *)RawADC_16b_Buffer_A;

//        pU16ExtADCRaw = (uint16_t *)RawADC_16b_Buffer_A;

        bADCRawNew |= Raw_New_ExtADC_Half;

        //挑一组出来作为实时数据
      /*  for (i = 0; i < ExtADC_Chs; i++)
            Realtime_RawData[i] = *pU16ExtADCRaw++;
        GyroTemp = Realtime_RawData[ExtADC_Chs - 1];
				*/
    }
    else if (ExtADC_Full == RAM_DMA_Sign_Full)
    {
        // EventStopA(0);
        // EventStartB(0);

        ExtADC_Full = RAM_DMA_Sign_Empty;

        // 原始数据，24bit --> 16bit
        RawData_24b_16b((uint8_t *)(RawADC_24b_Buffer_B),
                        (int16_t *)(RawADC_16b_Buffer_B),
                        ExtADC_Chs,
                        ExtADC_RecordLength_Each_Buffer);

        pU8ExtADCRaw = (uint8_t *)RawADC_16b_Buffer_B;

        //pU16ExtADCRaw = (uint16_t *)RawADC_16b_Buffer_B;

        bADCRawNew |= Raw_New_ExtADC_Full;
    }

    // ------------ IntADC ------------

    if (IntADC_Full == RAM_DMA_Sign_HalfFull)
    {
        // EventStartC(0);
        // EventStopD(0);

        IntADC_Full = RAM_DMA_Sign_Empty;

        pU8IntADCRaw = (uint8_t *)IntADC_16b_Buffer_A;
        pU16IntADCRaw = (uint16_t *)IntADC_16b_Buffer_A;

        bADCRawNew |= Raw_New_IntADC_Half;

        //挑一组出来作为实时数据
       /* for (i = 0; i < IntADC_Chs; i++)
            Realtime_RawData[ExtADC_Chs + i] = *pU16IntADCRaw++;
*/
       // bNew_Realtime_RawData = 1; // 提示有新的原始数据
    }
    else if (IntADC_Full == RAM_DMA_Sign_Full)
    {
        // EventStopC(0);
        // EventStartD(0);
        IntADC_Full = RAM_DMA_Sign_Empty;

        pU8IntADCRaw = (uint8_t *)IntADC_16b_Buffer_B;
        pU16IntADCRaw = (uint16_t *)IntADC_16b_Buffer_B;

        bADCRawNew |= Raw_New_IntADC_Full;
    }

    // 如果有数据了，就存进flash
    if ((bADCRawNew == (Raw_New_ExtADC_Half | Raw_New_IntADC_Half)) || // half
        (bADCRawNew == (Raw_New_ExtADC_Full | Raw_New_IntADC_Full)))   // full
    {
        if (MixSensorDataWriteOffset == 0 || MixSensorDataWriteOffset == LogicPageSize) // new page
        {
            Init_Page_IndexData(); // 应该执行不到
        }

        for (j = 0; j < IntADC_RecordLength_Each_Buffer; j++)
        {
            for (i = 0; i < (ExtADC_Chs - 1) * 2; i++) // 最后一个ch是GyroTemp，不存储
            {
                MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = *pU8ExtADCRaw++;
            }

            //   [ExtADC_Chs-1]
            pU8ExtADCRaw+=2;           

            for (i = 0; i < IntADC_Chs * 2; i++)
            {
                MixSensorDataWriteBuffer[MixSensorDataWriteOffset++] = *pU8IntADCRaw++;
            }

            if (MixSensorDataWriteOffset >= sizeof(MixSensorDataWriteBuffer))
            {
                Storage_APP_Add_MixSensorRaw_Log(MixSensorDataWriteBuffer, sizeof(MixSensorDataWriteBuffer));
                MixSensorDataWriteOffset = 0;
            }
        }

        bADCRawNew = Raw_New_Clear;
    }
}

/*********** (C) COPYRIGHT 2022  skyhigh  *****END OF FILE****/

/**
 * @file user_rtc.c
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-07-16
 * @copyright Copyright (c) 2020  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "user_rtc.h"
#include "RV-3029-C2.h"

/* Private define ------------------------------------------------------------*/

#define Using_OS 1

#define Using_ExRTC 1

extern RTC_HandleTypeDef hrtc;

#if Using_OS
#include "cmsis_os2.h"

osMutexId_t mutex_rtc;

const osMutexAttr_t RTC_Mutex_attr = {
    "RTCMutex",                            // human readable mutex name
    osMutexRecursive | osMutexPrioInherit, // attr_bits
    NULL,                                  // memory for control block
    0U                                     // size for control block
};

#endif
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
uint32_t RTC_STAMP;

// 实时时钟
uint8_t rtc_val[8];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Configure the current time and date.
 * @param  None
 * @retval None
 */
static void RTC_CalendarConfigDefault(void)
{
#if Using_OS
    osMutexAcquire(mutex_rtc, osWaitForever);
#endif

#if Using_ExRTC
    uint8_t datetime[6];
    datetime[0] = 20;             // year
    datetime[1] = RTC_MONTH_JULY; // month
    datetime[2] = 15;             // days
    datetime[3] = 12;             // hour
    datetime[4] = 20;             // minute
    datetime[5] = 0;              // second
    RV3029_Set_RTC(datetime);
#else
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /*##-1- Configure the Date #################################################*/
    /* Set Date:  */
    sdatestructure.Year = 20;
    sdatestructure.Month = RTC_MONTH_JULY;
    sdatestructure.Date = 15;
    sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

    if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
        // Error_Handler();
    }

    /*##-2- Configure the Time #################################################*/
    /* Set Time: 02:00:00 */
    stimestructure.Hours = 12;
    stimestructure.Minutes = 20;
    stimestructure.Seconds = 0x00;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
        // Error_Handler();
    }
#endif

    /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, RTC_Configed_Sign);

#if Using_OS
    osMutexRelease(mutex_rtc);
#endif
}

/* Public functions ---------------------------------------------------------*/

void Init_RTC(void)
{
#if Using_OS
    mutex_rtc = osMutexNew(&RTC_Mutex_attr);
#endif
}

// 读出 年(实际年-2000) 月 日 时 分 秒
void Get_RTC_DDHHMMSS(uint8_t *pOut)
{
    uint32_t tmp;

#if Using_OS
    osMutexAcquire(mutex_rtc, osWaitForever);
#endif

#if Using_ExRTC

    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != RTC_Configed_Sign)
    {
        RTC_CalendarConfigDefault();
    }

    RV3029_Get_RTC(pOut);

    tmp = pOut[0];
    RTC_STAMP = tmp << 26; // years  6位

    tmp = pOut[1];
    RTC_STAMP |= tmp << 22; // months 4位

    tmp = pOut[2];
    RTC_STAMP |= tmp << 17; // days 5位

    tmp = pOut[3];
    RTC_STAMP |= tmp << 12; // hours 5位

    tmp = pOut[4];
    RTC_STAMP |= tmp << 6; // minute 6位

    tmp = pOut[5];
    RTC_STAMP |= tmp; // seconds 6位

#else

    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;

    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != RTC_Configed_Sign)
    {
        RTC_CalendarConfigDefault();
    }

    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

    tmp = sdatestructureget.Year;
    pOut[0] = (uint8_t)tmp;
    RTC_STAMP = tmp << 26; //  6位

    tmp = sdatestructureget.Month;
    pOut[1] = (uint8_t)tmp;
    RTC_STAMP |= tmp << 22; //  4位

    tmp = sdatestructureget.Date;
    pOut[2] = (uint8_t)tmp;
    RTC_STAMP |= tmp << 17; //  5位

    tmp = stimestructureget.Hours;
    pOut[3] = (uint8_t)tmp;
    RTC_STAMP |= tmp << 12; //  5位

    tmp = stimestructureget.Minutes;
    pOut[4] = (uint8_t)tmp;
    RTC_STAMP |= tmp << 6; //  6位

    tmp = stimestructureget.Seconds;
    pOut[5] = (uint8_t)tmp;
    RTC_STAMP |= tmp; //  6位
#endif

#if Using_OS
    osMutexRelease(mutex_rtc);
#endif
}

// 读出 年(实际值-2000) 月 日 时 分 秒
void Get_RTC(uint8_t *pOut)
{
    Get_RTC_DDHHMMSS(pOut);
}

// 写入 年(实际值-2000) 月 日 时 分 秒
void Set_RTC(uint8_t *pIn)
{
#if Using_OS
    osMutexAcquire(mutex_rtc, osWaitForever);
#endif

#if Using_ExRTC
    RV3029_Set_RTC(pIn);
#else

    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    stimestructure.Hours = pIn[3];
    stimestructure.Minutes = pIn[4];
    stimestructure.Seconds = pIn[5];
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);

    sdatestructure.Year = pIn[0];
    sdatestructure.Month = pIn[1];
    sdatestructure.Date = pIn[2];
    sdatestructure.WeekDay = RTC_WEEKDAY_WEDNESDAY;

    HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);
#endif

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, RTC_Configed_Sign);

#if Using_OS
    osMutexRelease(mutex_rtc);
#endif
}

/**
 * @brief  设置复位类型，人为复位之前设置
 * @param  cfg              My Param doc
 */
void Set_Reset_Cfg(uint8_t cfg)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, Reset_Cfg_Configed_Sign_A);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, cfg);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, Reset_Cfg_Configed_Sign_A);
}

/**
 * @brief  设置复位类型
 * @return uint8_t
 */
uint8_t Get_Reset_Cfg(void)
{
    if (
        (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == Reset_Cfg_Configed_Sign_A) &&
        (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3) == Reset_Cfg_Configed_Sign_A))
    {
        return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    }

    return 0x00;
}

/*********** (C) COPYRIGHT 2020  skyhigh  *****END OF FILE****/

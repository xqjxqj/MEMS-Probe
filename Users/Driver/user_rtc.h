/**
 * @file user_rtc.h
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-07-16
 * @copyright Copyright (c) 2020  CAS 
 */
#ifndef __USER_RTC_H_
#define __USER_RTC_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Public define --------------------------------------------------------—----*/


/* Typedef ----------------------------------------------------------————————-*/
#define RTC_Configed_Sign   0xaabb
#define Reset_Cfg_Configed_Sign_A   0x5588

typedef enum
{
    Reset_Type_Normal=0x00,
    Reset_Type_HighSpeed=0x01,

    Reset_Type_Undefine
} Reset_Type;

/* Public macro -------------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/
extern uint32_t RTC_STAMP;
extern uint8_t rtc_val[8];

/* Public functions ---------------------------------------------------------*/
extern void Init_RTC(void);
extern void Get_RTC_DDHHMMSS(uint8_t *pOut);
extern void Get_RTC(uint8_t *pOut);
extern void Set_RTC(uint8_t *pIn);

extern void Set_Reset_Cfg(uint8_t cfg);
extern uint8_t Get_Reset_Cfg(void);


#endif	 // __USER_RTC_H_ 

/*********** (C) COPYRIGHT 2020 skyhigh *****END OF FILE****/

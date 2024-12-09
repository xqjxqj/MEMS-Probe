/**
 * @file Untitled-1
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-08-03
 * @copyright Copyright (c) 2020  CAS 
 */
#ifndef __RV_3029_C2_H_
#define __RV_3029_C2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Public define --------------------------------------------------------—----*/

/* Typedef ----------------------------------------------------------————————-*/
struct TimeSpec
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t weekdays;
    uint8_t months;
    uint8_t years;
};

#define RV3029_Reg_Control_1 0x00
#define RV3029_Reg_Control_INT 0x01
#define RV3029_Reg_Control_INT_Flag 0x02
#define RV3029_Reg_Control_Status 0x03
#define RV3029_Reg_Control_Reset 0x04

#define RV3029_Bit_Control_1_Clk_Int 0x80
#define RV3029_Bit_Control_1_TD1 0x40
#define RV3029_Bit_Control_1_TD0 0x20
#define RV3029_Bit_Control_1_SROn 0x10
#define RV3029_Bit_Control_1_EERE 0x08
#define RV3029_Bit_Control_1_TAR 0x04
#define RV3029_Bit_Control_1_TE 0x02
#define RV3029_Bit_Control_1_WE 0x01

#define RV3029_Bit_Control_INT_SRIE 0x10
#define RV3029_Bit_Control_INT_V2IE 0x08
#define RV3029_Bit_Control_INT_V1IE 0x04
#define RV3029_Bit_Control_INT_TIE 0x02
#define RV3029_Bit_Control_INT_AIE 0x01

#define RV3029_Bit_Control_INT_Flag_SRF 0x10
#define RV3029_Bit_Control_INT_Flag_V2IF 0x08
#define RV3029_Bit_Control_INT_Flag_V1IF 0x04
#define RV3029_Bit_Control_INT_Flag_TF 0x02
#define RV3029_Bit_Control_INT_Flag_AF 0x01

#define RV3029_Bit_Control_Status_EEbusy 0x80
#define RV3029_Bit_Control_Status_PON 0x20
#define RV3029_Bit_Control_Status_SR 0x10
#define RV3029_Bit_Control_Status_V2F 0x08
#define RV3029_Bit_Control_Status_V1F 0x04

#define RV3029_Bit_Control_Reset_SysR 0x10

#define RV3029_Reg_Seconds 0x08
#define RV3029_Reg_Minutes 0x09
#define RV3029_Reg_Hours 0x0A
#define RV3029_Reg_Days 0x0B
#define RV3029_Reg_Weekdays 0x0C
#define RV3029_Reg_Months 0x0D
#define RV3029_Reg_Years 0x0E

#define RV3029_Reg_Second_Alarm 0x10
#define RV3029_Reg_Minute_Alarm 0x11
#define RV3029_Reg_Hour_Alarm 0x12
#define RV3029_Reg_Days_Alarm 0x13
#define RV3029_Reg_Weekday_Alarm 0x14
#define RV3029_Reg_Months_Alarm 0x15
#define RV3029_Reg_Year_Alarm 0x16

#define RV3029_Reg_Timer_Low 0x18
#define RV3029_Reg_Timer_High 0x19

#define RV3029_Reg_Temperature 0x20

#define RV3029_EEPROM_User_1 0x28
#define RV3029_EEPROM_User_2 0x29

#define RV3029_Reg_EEPROM_Contr 0x30
#define RV3029_Reg_Xtal_Offset 0x31
#define RV3029_Reg_Xtal_Coef 0x32
#define RV3029_Reg_Xtal_T0 0x33

#define RV3029_User_RAM_First 0x38
#define RV3029_User_RAM_Last 0x3F

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/

extern float RV3029_Get_Temp(void);

extern int8_t RV3029_Get_RTC(uint8_t *time);
extern int8_t RV3029_Set_RTC(uint8_t *time);

#endif // __RV_3029_C2_H_

/*********** (C) COPYRIGHT 2020 skyhigh *****END OF FILE****/

/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-08-03
 * @copyright Copyright (c) 2020  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "RV-3029-C2.h"

/* Private define ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

#define RV3029_ADDR_W 0xAC
#define RV3029_ADDR_R 0xAD

#define BCD2BIN(val) ((((val)&0x0f) + ((val) >> 4) * 10))
#define BIN2BCD(val) ((((val) / 10) << 4) + (val) % 10)

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
int8_t rv3029_read(uint8_t regaddr, uint8_t bytes, uint8_t *val)
{
#if 0

#else

    return (int8_t)HAL_I2C_Mem_Read(&hi2c1, RV3029_ADDR_R, regaddr, I2C_MEMADD_SIZE_8BIT, val, bytes, 100000);
    return 1;
#endif
}

int8_t rv3029_write(uint8_t regaddr, uint8_t bytes, uint8_t *val)
{
#if 0

#else
    return (int8_t)HAL_I2C_Mem_Write(&hi2c1, RV3029_ADDR_W, regaddr, I2C_MEMADD_SIZE_8BIT, val, bytes, 100000);
    return 1;
#endif
}

/* Private functions ---------------------------------------------------------*/

float RV3029_Get_Temp(void)
{
    uint8_t temp;
    float ret;

    rv3029_read(RV3029_Reg_Temperature, 1, &temp);
    {
        ret = (int8_t)temp;
        return (ret - 60.0f);
    }

    return (-100.0f);
}

int8_t RV3029_Get_Time(struct TimeSpec *time)
{
    uint8_t raw[7];
    if (rv3029_read(0x08, 7, raw) > 0)
        return 0x00;

    time->seconds = BCD2BIN(raw[0]);
    time->minutes = BCD2BIN(raw[1]);
    time->hours = BCD2BIN(raw[2]);
    time->days = BCD2BIN(raw[3]);
    time->weekdays = raw[4];
    time->months = BCD2BIN(raw[5]);
    time->years = BCD2BIN(raw[6]);

    return 0x01;
}

int8_t RV3029_Set_Time(struct TimeSpec *time)
{
    uint8_t raw[7];
    raw[0] = BIN2BCD(time->seconds);
    raw[1] = BIN2BCD(time->minutes);
    raw[2] = BIN2BCD(time->hours);
    raw[3] = BIN2BCD(time->days);
    raw[4] = time->weekdays;
    raw[5] = BIN2BCD(time->months);
    raw[6] = BIN2BCD(time->years);
    return rv3029_write(0x08, 7, raw);
}

int8_t RV3029_Get_RTC(uint8_t *time)
{
    uint8_t raw[7];
    if (rv3029_read(RV3029_Reg_Seconds, 7, raw) > 0)
        return 0x00;

    time[5] = BCD2BIN(raw[0]); // second
    time[4] = BCD2BIN(raw[1]); // minute
    time[3] = BCD2BIN(raw[2]); // hour

    time[2] = BCD2BIN(raw[3]); // day
    time[1] = BCD2BIN(raw[5]); // month
    time[0] = BCD2BIN(raw[6]); // year

    return 0x01;
}

int8_t RV3029_Set_RTC(uint8_t *time)
{
    uint8_t raw[7];
    raw[0] = BIN2BCD(time[5]);
    raw[1] = BIN2BCD(time[4]);
    raw[2] = BIN2BCD(time[3]);
    raw[3] = BIN2BCD(time[2]);
    raw[4] = 0x01;
    raw[5] = BIN2BCD(time[1]);
    raw[6] = BIN2BCD(time[0]);
    return rv3029_write(0x08, 7, raw);
}

/* Public functions ---------------------------------------------------------*/

/*********** (C) COPYRIGHT 2020  skyhigh  *****END OF FILE****/

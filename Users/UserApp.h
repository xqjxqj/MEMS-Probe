/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-07-14
 * @copyright Copyright (c) 2020  CAS
 */
#ifndef __UserAPP_H_
#define __UserAPP_H_

/* Includes ------------------------------------------------------------------*/

/* Public define --------------------------------------------------------—----*/

/* Typedef ----------------------------------------------------------————————-*/
typedef struct
{
    float AccLxP;
    float AccLxN;

    float AccLyP;
    float AccLyN;

    float AccLzP;
    float AccLzN;

    float AccL45xP;
    float AccL45xN;

    float AccL45yP;
    float AccL45yN;

    float AccL45zP;
    float AccL45zN;

    float AccHxP;
    float AccHxN;

    float AccHyP;
    float AccHyN;

    float AccHzP;
    float AccHzN;

} DiffAccDataTypeDef;

typedef struct
{
    float AccLx;
    float AccLy;
    float AccLz;

    float AccL45x;
    float AccL45y;
    float AccL45z;

    float AccHx;
    float AccHy;
    float AccHz;

} AccDataTypeDef;

typedef struct
{
    float MagX;
    float MagY;
    float MagZ;
} MagDataTypeDef;

typedef struct
{
    float VibX;
    float VibY;
    float VibZ;
} VibDataTypeDef;

typedef struct
{
    AccDataTypeDef AccData;
    MagDataTypeDef MagData;
    VibDataTypeDef VibData;

    float fRotate;
    float fTemperature;
    float Current;
    float Voltage1;
    float Voltage2;
} AllDataTypeDef;


typedef struct
{
    float ADC_B_Ref;   // B: Reference
    float AccLRPRef;   // B: adxl354_1
    float AccLRNRef;   // B: adxl354_2
    float AccLR45PRef; // B: adxl354_3
    float AccLR45NRef; // B: adxl354_4
    float AccVibPRef;  // B: adxl356C
    float ADC_A_Ref;   // A: Reference
    float AccHRPRef;   // A: adxl356B_1
    float AccHRNRef;   // A: adxl356B_2
    float GyroRef;     // A: Gyro Zero level
    float TempRef;     // A:temperature ref at 25C
}AccRefTypeDef;

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/

extern void
Creat_user_app_init_task(void);

#endif // __UserAPP_H_

/*********** (C) COPYRIGHT 2020 skyhigh *****END OF FILE****/

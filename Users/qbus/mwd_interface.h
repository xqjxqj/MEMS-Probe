/**
 * @file mwd_interface.h
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2024-09-15
 * @copyright Copyright (c) 2024  CAS 
 */
#ifndef __MWD_INTERFACE_H
#define __MWD_INTERFACE_H


/* Includes ------------------------------------------------------------------*/


/* Public define --------------------------------------------------------—----*/




typedef struct 
{
    float update; // 0=未更新，1=已更新
    float gru; // 上伽马  GVA
    float grd; // 下伽马  GVB
    float inc; // 井斜    GVC
    float azi; // 方位    GVD
    float res; // 浅电阻率
    float red; // 深电阻率
    float groll; // 工具面角
    float rot; //   转速
}mwd_data_typedef;


typedef struct 
{
    float update; // 0=未更新，1=已更新Inc Azi Tf
    float inc;//井斜
	  float azi;//方位
    float Tf; // 工具面角
}mwd2_data_typedef;

/* Typedef ----------------------------------------------------------————————-*/


/* Public macro -------------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/
extern mwd_data_typedef mwd_data_tx,mwd_data_rx;
extern mwd2_data_typedef mwd2_data_tx;
extern unsigned short MWD_GRTotal;
/* Public functions ---------------------------------------------------------*/
extern void Init_MWD_APP();






#endif	 // __MWD_INTERFACE_H 

/*********** (C) COPYRIGHT 2024 skyhigh *****END OF FILE****/

/**
 * @file CMDTask.h
 * @brief
 * @author xqj (xqj8595@163.com)
 * @version 1.0
 * @date 2022-04-06
 * @copyright Copyright (c) 2022  CAS
 */
#ifndef __CMDTASK_H
#define __CMDTASK_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Public define --------------------------------------------------------—----*/

/* Typedef ----------------------------------------------------------————————-*/

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
extern volatile uint8_t bUSBStorage;
/* Public functions ---------------------------------------------------------*/

extern void RePowerMyself(void);
extern void init_cmd_ringbuffer(void);
extern uint8_t Decode_DL_CMD(void);

extern void Send_Realtime_RawData(void);
extern void Com_Send_Data(unsigned char *pSrc, unsigned short Size);

//extern void Creat_user_app_CMD_task(void);

#endif // __CMDTASK_H

/*********** (C) COPYRIGHT 2022 skyhigh *****END OF FILE****/

/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-07-14
 * @copyright Copyright (c) 2020  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ringbuffer.h"

/* Private define ------------------------------------------------------------*/

// stack size must be multiple of 8 Bytes
#define USER_APP_CMD_STK_SZ (4096U)
uint64_t user_app_CMD_stk[USER_APP_CMD_STK_SZ / 8];
const osThreadAttr_t user_app_CMD_attr = {
    .stack_mem = &user_app_CMD_stk[0],
    .stack_size = sizeof(user_app_CMD_stk)};

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t cmd_bytes[256];
struct rt_ringbuffer rt_cmd;

/* Public variables ---------------------------------------------------------*/

// uint8_t NewCMDPack = 0; // 收到总线上新的数据包

volatile uint8_t NewCMD = 0; // 收到总线上新的命令

volatile uint8_t bRealtimeUpdate = 0; // =1 需要实时上传
volatile uint8_t bUSBStorage = 0;     // =1 需要打开USB存储器

uint8_t SendBuf[128];
uint16_t SendLength;

extern unsigned short Realtime_RawData[ExtADC_Chs + IntADC_VT_Chs]; // 存一组实时数据
extern unsigned char bNew_Realtime_RawData;

/* Private function prototypes -----------------------------------------------*/
void RePowerMyself(void)
{
   HAL_NVIC_SystemReset();
}
/* Private functions ---------------------------------------------------------*/

// 串口发送数据
void Com_Send_Data(unsigned char *pSrc, unsigned short Size)
{
   /*
#if OnebusCOM2
   Uart2_Write(pSrc, Size);
#else
   Uart1_Write(pSrc, Size);
#endif
   */
   Uart3_Write(pSrc, Size);
}

// 直接发送命令
void Comd_Send(void)
{
   Uart3_Write(SendBuf, SendLength);
}

void init_cmd_ringbuffer(void)
{
   rt_ringbuffer_init(&rt_cmd, cmd_bytes, sizeof(cmd_bytes));
}

/**
 * @brief  处理收到的字节
 *
 */
void Recvive_Cmd_Bytes(void)
{
   unsigned short i;
   unsigned char tmpC;
   unsigned short size;

   NewCMD = DLCmd_PackType_CMD_Undefine;

   size = rt_ringbuffer_data_len(&rt_cmd);

   for (i = 0; i < size; i++)
   {
      if (rt_ringbuffer_getchar(&rt_cmd, &tmpC) > 0)
      {
         // SendBuf[SendLength++] = tmpC;
         NewCMD = Receive_Byte(tmpC);

         if (NewCMD != DLCmd_PackType_CMD_Undefine)
         {
            break;
         }
      }
   }
}

/* Public functions ---------------------------------------------------------*/

/**
 * @brief  解析下传命令
 *
 * @return uint8_t
 */
uint8_t Decode_DL_CMD(void)
{
   uint32_t u32t;
   uint8_t U8tmp[8];

   switch (NewCMD)
   {
   case (DLCmd_PackType_RealtimeData): //
   {
      bRealtimeUpdate = LastDLCmd_Payload[0];

      break;
   }
   case (DLCmd_PackType_SetRTC): //
   {
      Set_RTC(LastDLCmd_Payload);

      break;
   }
   case (DLCmd_PackType_GetRTC): //
   {
      // 默认在主循环内已经读取时钟
      SendLength = Gen_Tx_Pack_RTC(rtc_val, SendBuf);
      Comd_Send(); // 命令发送出去
      break;
   }
   case (DLCmd_PackType_Get_File_Infor): //
   {
      // 获取文件大小
      u32t = Storage_APP_GetSize_MixSensorRaw_Log();
      // U8tmp[0] = (uint8_t)(u32t >> 24);
      // U8tmp[1] = (uint8_t)(u32t >> 16);
      // U8tmp[2] = (uint8_t)(u32t >> 8);
      // U8tmp[3] = (uint8_t)(u32t);

      SendLength = Gen_Tx_Pack_FileInfor(u32t, SendBuf);

      Comd_Send(); // 命令发送出去
      break;
   }
   case (DLCmd_PackType_Get_File_Data): //
   {

      break;
   }
   case (DLCmd_PackType_Del_File): //
   {
      Storage_APP_Delete_MixSensorRaw_Log();
      break;
   }
   case (DLCmd_PackType_USB_Storage): //
   {
      bUSBStorage = LastDLCmd_Payload[0];
      break;
   }
   case (DLCmd_PackType_Reset):
   {
      HAL_NVIC_SystemReset(); // 重新启动
      break;
   }
   default:
   {

      break;
   }
   }
   NewCMD = DLCmd_PackType_CMD_Undefine; // 处理完毕
   return 0x01;
}

void Send_Realtime_RawData(void)
{
   if (bRealtimeUpdate)
   {
      if (bNew_Realtime_RawData) //XQJ
      {
         bNew_Realtime_RawData = 0;

         SendLength = Gen_Tx_Pack_RawADC(Realtime_RawData, SendBuf);
				//SendLength = Gen_Tx_Pack_RawADC(RawExtADCData, SendBuf);
         Comd_Send(); // 命令发送出去
      }
   }
}

/**
 * @brief  收到总线数据后处理
 * @param  pSrc             My Param doc
 * @param  size             My Param doc
 */
void User_UART_RxCpltCallback_3(uint8_t *pSrc, uint16_t size) // 将DMA的buffer中的数据放入ringbuffer
{
   rt_ringbuffer_put(&rt_cmd, pSrc, size);
}

void User_UART_IDLECallback_3(void) // 将ringbuffer中的数据取出来放入SendBuf
{
#if 0
   unsigned short i;
   unsigned char tmpC;
   unsigned short size;

   size = rt_ringbuffer_data_len(&rt_cmd);

   for (i = 0; i < size; i++)
   {
      if (rt_ringbuffer_getchar(&rt_cmd, &tmpC) > 0)
      {
         SendBuf[SendLength++] = tmpC;
      }
   }
#else
   Recvive_Cmd_Bytes();
#endif
}

#if 0
__NO_RETURN void user_app_CMD(void *arg)
{
   uint8_t newset;

   uint32_t i, j, m, SendoutCounter;
   uint8_t u8Buf[128];
   uint16_t sVal[20];
   uint16_t size;

   uint8_t CmdPacketIndex = 0;

   uint8_t bAutoReport = 0; // 自动报告

   uint8_t *p;
   uint8_t cmdpackreceiver;

   (void)arg;

   osDelay(1000);

   init_cmd_ringbuffer();
   User_UART_Start(Huart2_Index);

   // SendLength = sprintf((char *)SendBuf, "%s", "helll STM32G474!");

   SendBuf[0] = 0x55;
   SendBuf[1] = 0xaa;
   SendLength = 2;

   Com_Send_Data(SendBuf, SendLength);
   SendLength = 0;

   while (1)
   {
      LED_R_FLASH;

      osDelay(500);

      if (SendLength > 0) //通过判断SendLength来确定SendBuf中是否为空，不为空则将SendBuf中数据通过串口发出去
      {
         Com_Send_Data(SendBuf, SendLength);
         SendLength = 0;
      }

      Get_RTC(rtc_val);
   }
}


void Creat_user_app_CMD_task(void)
{
   osThreadNew(user_app_CMD, NULL, &user_app_CMD_attr);
}

#endif

/*********** (C) COPYRIGHT 2020  skyhigh  *****END OF FILE****/

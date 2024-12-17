/**
 * @file mwd_interface.c
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2024-09-15
 * @copyright Copyright (c) 2024  CAS
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

/* Private variables ---------------------------------------------------------*/
// 发送数据

unsigned char MWD_Tx_Buffer[128]__attribute__((section(".ARM.__at_0x20000000")));
unsigned short MWD_Tx_Length = 0;
unsigned short MWD_GRTotal=0;
// rt_ringbuffer
unsigned char mwd_rawbyte_bytes[256];
struct rt_ringbuffer rt_mwd_rawbyte;

/* Public variables ---------------------------------------------------------*/
mwd_data_typedef mwd_data_tx, mwd_data_rx;
mwd2_data_typedef mwd2_data_tx;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/
void Init_MWD_APP(void)
{
    rt_ringbuffer_init(&rt_mwd_rawbyte, mwd_rawbyte_bytes, sizeof(mwd_rawbyte_bytes));
    MWD_Tx_Length = 0; //
    mwd_data_tx.update=0;
	  mwd2_data_tx.update=1;
	  mwd_data_rx.update=0;
    User_UART_Start(Huart1_Index);
}

void Gen_MWD_Reply(void)
{
    char str[128];
	  if (mwd_data_tx.update == 1)
    {
        // GVA=0010;GVB=0020;RES=0050;RED=0100;GVC=0030;GVD=0120;
        sprintf(str, "GVE=%d;GVF=%d;GVG=%d;RES=%d;RED=%d;GVC=%d;GVD=%d;", (int)(mwd_data_tx.gru),
                (int)(mwd_data_tx.grd),
											MWD_GRTotal,
                (int)(mwd_data_tx.res),
                (int)(mwd_data_tx.red),
                (int)(mwd_data_tx.inc),
                (int)(mwd_data_tx.azi));
        
			/*
			        sprintf(str, "GVA=%d;GVB=%d;GVC=%d;GVD=%d;", (int)(mwd_data_tx.gru),
                (int)(mwd_data_tx.grd),
                (int)(mwd_data_tx.inc*10.0),
                (int)(mwd_data_tx.azi));
			*/
              //mwd_data_tx.update = 0;
				MWD_Tx_Length=GenQBUS_ReplyPack(QBUS_defaut_Client, QBUS_defaut_Server, str, MWD_Tx_Buffer);
				HAL_UART_Transmit_DMA(&huart1,MWD_Tx_Buffer,MWD_Tx_Length);
    }
    else if (mwd2_data_tx.update == 1)
    {
        // GVA=0010;GVB=0020;RES=0050;RED=0100;GVC=0030;GVD=0120;
        sprintf(str, "GV0=%d;GV1=%d;", (int)((mwd2_data_tx.inc)*10), //GV1=%.2f;
                (int)((mwd2_data_tx.azi)*10)
			          );
              mwd2_data_tx.update = 0;
				MWD_Tx_Length=GenQBUS_ReplyPack(QBUS_defaut_Client, QBUS_defaut_Server, str, MWD_Tx_Buffer);
				HAL_UART_Transmit_DMA(&huart1,MWD_Tx_Buffer,MWD_Tx_Length);
    }
    else if(mwd_data_rx.update == 1)
    {
        sprintf(str, "GVE=%d;GVF=%d;GVG=%d;RES=%d;RED=%d;GVC=%d;GVD=%d;", (int)(mwd_data_rx.gru),
                (int)(mwd_data_rx.grd),
										MWD_GRTotal,
                (int)(mwd_data_rx.res),
                (int)(mwd_data_rx.red),
                (int)(mwd_data_rx.inc),
                (int)(mwd_data_rx.azi));
					
			/*
			        sprintf(str, "GVA=%d;GVB=%d;GVC=%d;GVD=%d;", (int)(mwd_data_rx.gru),
                (int)(mwd_data_rx.grd),
                (int)(mwd_data_rx.inc*10.0),
                (int)(mwd_data_rx.azi));
			*/
			        mwd_data_rx.update = 0;
				MWD_Tx_Length=GenQBUS_ReplyPack(QBUS_defaut_Client, QBUS_defaut_Server, str, MWD_Tx_Buffer);
				HAL_UART_Transmit_DMA(&huart1,MWD_Tx_Buffer,MWD_Tx_Length);
								
    }
    else
		{   
			  HAL_UART_Transmit_DMA(&huart1,MWD_Tx_Buffer,MWD_Tx_Length);
				//MWD_Tx_Buffer[0]=0x04;
				//
			//HAL_UART_Transmit_DMA(&huart1,MWD_Tx_Buffer,1);
		}

}

/**
 * @brief
 * @param  pSrc             My Param doc
 * @param  size             My Param doc
 */
void User_UART_RxCpltCallback_1(uint8_t *pSrc, uint16_t size) //
{
    rt_ringbuffer_put(&rt_mwd_rawbyte, pSrc, size);
}

void User_UART_IDLECallback_1(void) //
{
    unsigned short len, i;
    unsigned char _byte;

    len = rt_ringbuffer_data_len(&rt_mwd_rawbyte);

    for (i = 0; i < len; i++)
    {
        rt_ringbuffer_getchar(&rt_mwd_rawbyte, &_byte);
        if ((QBUS_Received_One_Byte(_byte) == QBUS_Message_Polling) && (default_QBUS_Clinet_ID == QBUS_CallingID)) // 询问本仪器要数据
        {
            Gen_MWD_Reply();
            rt_ringbuffer_reset(&rt_mwd_rawbyte);
        }
    }
}

/*********** (C) COPYRIGHT 2024  skyhigh  *****END OF FILE****/

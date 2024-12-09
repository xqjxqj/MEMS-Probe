
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_usart_H
#define __user_usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

#define Using_RTO  1

#define UART_RX_BUF_SIZE   32

// typedef struct
// {
// 	uint8_t RxBuf[UART_RX_BUF_SIZE];
// 	uint8_t RxBufSize;
// 	uint8_t RxOffset;
// 	uint8_t *pTxBuf;
// 	uint8_t TxBufSize;
// 	uint8_t TxOffset;
// }User_Usart_TypeDef;

typedef enum   
{
	Huart1_Index=0,
	Huart2_Index,
	Huart3_Index,
//	Huart4_Index,
//	Huart5_Index,
//	Huart6_Index,
//	Huart7_Index,
	Huart_Size	
}Huart_Index_TypeDef;

//extern User_Usart_TypeDef  User_Usart_Buffer[Huart_Size];

extern uint8_t Uart_PackCpt_Sign[Huart_Size];

/* USER CODE END Private defines */

//void MX_UART7_Init(void);
//void MX_USART1_UART_Init(void);
//void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void UsartReceive_IDLE(UART_HandleTypeDef *huart) ;
void UsartReceive_TIMEOUT(UART_HandleTypeDef *huart);
void User_UART_Start(uint8_t index);
void User_UART_Stop(uint8_t index);

void User_UART_Clear_RxBuf(uint8_t index);

extern void Uart1_Write(uint8_t *pSrc, uint8_t size);
extern void Uart2_Write(uint8_t *pSrc,uint8_t size);
extern void Uart3_Write(uint8_t *pSrc,uint8_t size);

extern uint8_t bCheckPackArrived(uint8_t index);
extern void ResetPackArrivedSign(uint8_t index);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */



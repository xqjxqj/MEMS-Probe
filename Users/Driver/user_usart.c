

/* Includes ------------------------------------------------------------------*/
#include "user_usart.h"

/* USER CODE BEGIN 0 */

#include "stdio.h"
#include "string.h"
#include "ringbuffer.h"

__ALIGNED(32)
uint8_t Uart_DmaRcv_Buffer[Huart_Size][UART_RX_BUF_SIZE]__attribute__((section(".ARM.__at_0x20000100")));//96byte 0x60
//uint8_t Uart_DmaRcv_Buffer[Huart_Size][UART_RX_BUF_SIZE];
uint8_t Uart_DmaRcv_HalfCpt_Sign[Huart_Size];
uint8_t Uart_PackCpt_Sign[Huart_Size];

/* USER CODE END 0 */

#define Using_UART7 0

/* USER CODE BEGIN 1 */

/** @addtogroup  子函数
 * @{
 */

/**
 * @brief  开始 dma+idle
 * @param  uartHandle       串口
 * @param  pRcv             缓冲区指针
 * @param  size             缓冲区长度
 */
static void User_UART_Begin_DMAReceive(UART_HandleTypeDef *uartHandle, uint8_t *pRcv, uint16_t size)
{
	HAL_UART_Receive_DMA(uartHandle, pRcv, size);
}

/**
 * @brief  结束 dma+idle
 * @param  uartHandle
 */
static void User_UART_Stop_DMAReceive(UART_HandleTypeDef *uartHandle)
{
#if Using_RTO
	__HAL_UART_DISABLE_IT(uartHandle, UART_IT_RTO);
#else
	/*Disable the IDLE Interrupt*/
	__HAL_UART_DISABLE_IT(uartHandle, UART_IT_IDLE);

#endif

	HAL_UART_DMAStop(uartHandle);
}

/**
 1. @brief Stop the DMA Receive.
 2. @param huart: UART handle.
 3. @retval HAL status
  */
static HAL_StatusTypeDef HAL_UART_DMAStopRx(UART_HandleTypeDef *huart)
{
	/* Stop UART DMA Rx request if ongoing */
	if ((huart->RxState == HAL_UART_STATE_BUSY_RX) &&
		(HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)))
	{
		CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

		/* Abort the UART DMA Rx channel */
		if (huart->hdmarx != NULL)
		{
			HAL_DMA_Abort(huart->hdmarx);
		}

		// UART_EndRxTransfer(huart);
		/* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
		CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
		CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

		/* At end of Rx process, restore huart->RxState to Ready */
		huart->RxState = HAL_UART_STATE_READY;
	}

	return HAL_OK;
}

/**
 * @}
 */

/** @addtogroup   应用函数
 * @{
 */

/**
 * @brief  串口开始
 * @param  index            串口序列号
 */
void User_UART_Start(uint8_t index)
{
	if (index == Huart1_Index)
	{
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_ERR);
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_PE);

#if Using_RTO
		// config timeout
		HAL_UART_ReceiverTimeout_Config(&huart1, 100); // 时间单位是 1/波特率 ， 10个bit是一个byte时间
		HAL_UART_EnableReceiverTimeout(&huart1);

		__HAL_UART_ENABLE_IT(&huart1, UART_IT_RTO);
#else
		// // Enable the IDLE Interrupt
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
#endif

		User_UART_Begin_DMAReceive(&huart1, &(Uart_DmaRcv_Buffer[index][0]), UART_RX_BUF_SIZE);
	}
//	else if (index == Huart2_Index)
//	{
//		__HAL_UART_DISABLE_IT(&huart2, UART_IT_ERR);
//		__HAL_UART_DISABLE_IT(&huart2, UART_IT_PE);

//#if Using_RTO
//		// config timeout
//		HAL_UART_ReceiverTimeout_Config(&huart2, 100); // 时间单位是 1/波特率 ， 10个bit是一个byte时间
//		HAL_UART_EnableReceiverTimeout(&huart2);

//		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RTO);
//#else
//		// // Enable the IDLE Interrupt
//		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
//#endif

//		User_UART_Begin_DMAReceive(&huart2, &(Uart_DmaRcv_Buffer[index][0]), UART_RX_BUF_SIZE);
//	}
	else if (index == Huart3_Index)
	{
		__HAL_UART_DISABLE_IT(&huart3, UART_IT_ERR);
		__HAL_UART_DISABLE_IT(&huart3, UART_IT_PE);

#if Using_RTO
		// config timeout
		HAL_UART_ReceiverTimeout_Config(&huart3, 100); // 时间单位是 1/波特率 ， 10个bit是一个byte时间
		HAL_UART_EnableReceiverTimeout(&huart3);
		/*Enable the TIMEOUT Interrupt*/
		__HAL_UART_ENABLE_IT(&huart3, UART_IT_RTO);
#else
		// /*Enable the IDLE Interrupt*/
		__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
#endif

		User_UART_Begin_DMAReceive(&huart3, &(Uart_DmaRcv_Buffer[index][0]), UART_RX_BUF_SIZE);
	}

	//	else if (index == Huart4_Index)
	//	{
	//		__HAL_UART_DISABLE_IT(&huart4, UART_IT_ERR);
	//		__HAL_UART_DISABLE_IT(&huart4, UART_IT_PE);

	//#if Using_RTO										   // config timeout
	//		HAL_UART_ReceiverTimeout_Config(&huart4, 100); // 时间单位是 1/波特率 ， 10个bit是一个byte时间
	//		HAL_UART_EnableReceiverTimeout(&huart4);
	//		// Enable the TIMEOUT Interrupt
	//		__HAL_UART_ENABLE_IT(&huart4, UART_IT_RTO);
	//#else
	//		//  Enable the IDLE Interrupt
	//		__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
	//#endif

	//		User_UART_Begin_DMAReceive(&huart4, &(Uart_DmaRcv_Buffer[index][0]), UART_RX_BUF_SIZE);
	//	}

	// ----  To do ----
}

/**
 * @brief  串口停止
 * @param  index          串口序号
 */
void User_UART_Stop(uint8_t index)
{
	if (index == Huart1_Index)
	{
		User_UART_Stop_DMAReceive(&huart1);
	}
//	else if (index == Huart2_Index)
//	{
//		User_UART_Stop_DMAReceive(&huart2);
//	}
	else if (index == Huart3_Index)
	{
		User_UART_Stop_DMAReceive(&huart3);
	}
	//	else if (index == Huart4_Index)
	//	{
	//		User_UART_Stop_DMAReceive(&huart4);
	//	}

	// ----  To do ----
}

/**
 * @brief  清除串口缓冲区
 * @param  index     串口序号
 */
void User_UART_Clear_RxBuf(uint8_t index)
{
	memset(&(Uart_DmaRcv_Buffer[index][0]), 0, UART_RX_BUF_SIZE);
}

/**
 * @}
 */

/** @addtogroup  接收完成处理虚函数
 * @{
 */

/**
 * @brief  串口1接收完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_RxCpltCallback_1(uint8_t *pSrc, uint16_t size)
{
}

/**
 * @brief  串口2 DMA接收完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_RxCpltCallback_2(uint8_t *pSrc, uint16_t size)
{
}

/**
 * @brief  串口3 DMA接收完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_RxCpltCallback_3(uint8_t *pSrc, uint16_t size)
{
}

/**
 * @brief  串口4 DMA接收完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_RxCpltCallback_4(uint8_t *pSrc, uint16_t size)
{
}

/**
 * @brief  串口1 空闲完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_IDLECallback_1(void)
{
}

/**
 * @brief  串口2 空闲完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_IDLECallback_2(void)
{
}

/**
 * @brief  串口3 空闲完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_IDLECallback_3(void)
{
}

/**
 * @brief  串口4 空闲完成处理
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
__weak void User_UART_IDLECallback_4(void)
{
}

/**
 * @brief  DMA接收到缓冲区一半的处理
 * @param  huart 串口
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart1_Index][0])),
		// UART_RX_BUF_SIZE / 2);
		// 接收到数据处理函数
		User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][0]),
								   UART_RX_BUF_SIZE / 2);
		Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] = 1;
	}
//	else if (huart == &huart2)
//	{
//		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart2_Index][0])),
//		// UART_RX_BUF_SIZE / 2);
//		// 接收到数据处理函数
//		User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][0]),
//								   UART_RX_BUF_SIZE / 2);
//		Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] = 1;
//	}
	else if (huart == &huart3)
	{
		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart3_Index][0])),
		// UART_RX_BUF_SIZE / 2);
		// 接收到数据处理函数
		User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][0]),
								   UART_RX_BUF_SIZE / 2);
		Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] = 1;
	}
	//	else if (huart == &huart4)
	//	{
	//		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart4_Index][0])),
	//		// UART_RX_BUF_SIZE / 2);
	//		// 接收到数据处理函数
	//		User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][0]),
	//								   UART_RX_BUF_SIZE / 2);
	//		Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] = 1;
	//	}
}

/**
 * @brief  DMA接收到缓冲区全部的处理
 * @param  huart 串口
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart1_Index][UART_RX_BUF_SIZE / 2])),
		// UART_RX_BUF_SIZE / 2);
		// 接收到数据处理函数
		User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][UART_RX_BUF_SIZE / 2]),
								   UART_RX_BUF_SIZE / 2);
		Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] = 0;
	}
//	else if (huart == &huart2)
//	{
//		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart2_Index][UART_RX_BUF_SIZE / 2])),
//		// UART_RX_BUF_SIZE / 2);
//		// 接收到数据处理函数
//		User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][UART_RX_BUF_SIZE / 2]),
//								   UART_RX_BUF_SIZE / 2);
//		Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] = 0;
//	}
	else if (huart == &huart3)
	{
		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart3_Index][UART_RX_BUF_SIZE / 2])),
		// UART_RX_BUF_SIZE / 2);
		// 接收到数据处理函数
		User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][UART_RX_BUF_SIZE / 2]),
								   UART_RX_BUF_SIZE / 2);
		Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] = 0;
	}
	//	else if (huart == &huart4)
	//	{
	//		// SCB_InvalidateDCache_by_Addr((uint32_t *)(&(Uart_DmaRcv_Buffer[Huart4_Index][UART_RX_BUF_SIZE / 2])),
	//		// UART_RX_BUF_SIZE / 2);
	//		// 接收到数据处理函数
	//		User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][UART_RX_BUF_SIZE / 2]),
	//								   UART_RX_BUF_SIZE / 2);
	//		Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] = 0;
	//	}
}

/**
 * @}
 */

/** @addtogroup  接收处理中断
 * @{
 */

uint16_t len;
/**
 * @brief  串口总线空闲 接收处理
 * @param  huart     串口序号
 */
void UsartReceive_IDLE(UART_HandleTypeDef *huart)
{
	uint16_t i = 0;

	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) )
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart); //

		if (huart->Instance == USART1)
		{
			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][0]), len);
				}

				User_UART_IDLECallback_1();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收
			User_UART_Begin_DMAReceive(&huart1, &(Uart_DmaRcv_Buffer[Huart1_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] = 0;

			Uart_PackCpt_Sign[Huart1_Index] = 1;
		}
		#if UsUart2
        else if (huart->Instance == USART2)
		{
			//__HAL_UART_CLEAR_IDLEFLAG(huart); //

			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][0]), len);
				}

				User_UART_IDLECallback_2();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收

			User_UART_Begin_DMAReceive(&huart2, &(Uart_DmaRcv_Buffer[Huart2_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] = 0;

			Uart_PackCpt_Sign[Huart2_Index] = 1;
		}
		#endif
        else if (huart->Instance == USART3)
		{
			//__HAL_UART_CLEAR_IDLEFLAG(huart); //

			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][0]), len);
				}

				User_UART_IDLECallback_3();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收

			User_UART_Begin_DMAReceive(&huart3, &(Uart_DmaRcv_Buffer[Huart3_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] = 0;

			Uart_PackCpt_Sign[Huart3_Index] = 1;
		}
		/*else if (huart->Instance == UART4)
		{
			__HAL_UART_CLEAR_IDLEFLAG(huart); //

			i = huart->Instance->SR;
			i = huart->Instance->DR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] > 0)
				{

					// 接收到数据处理函数
					User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{

					User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][0]), len);
				}

				User_UART_IDLECallback_4();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收
			User_UART_Begin_DMAReceive(&huart4, &(Uart_DmaRcv_Buffer[Huart4_Index][0]), UART_RX_BUF_SIZE);
			Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] = 0;
			Uart_PackCpt_Sign[Huart4_Index] = 1;
		} */
	}

	// 清除掉各种中断源
	//	if (huart->Instance == USART2)
	//		HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
	//	else if (huart->Instance == USART3)
	//		HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
	//	else if (huart->Instance == UART4)
	//		HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
}

/**
 * @brief  串口总线空闲 接收处理
 * @param  huart     串口序号
 */
void UsartReceive_TIMEOUT(UART_HandleTypeDef *huart)
{
	uint16_t i = 0;

	if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RTOF) )
	{
		__HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_RTOF); //

		if (huart->Instance == USART1)
		{
			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_1(&(Uart_DmaRcv_Buffer[Huart1_Index][0]), len);
				}

				User_UART_IDLECallback_1();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收
			User_UART_Begin_DMAReceive(&huart1, &(Uart_DmaRcv_Buffer[Huart1_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart1_Index] = 0;

			Uart_PackCpt_Sign[Huart1_Index] = 1;
		}
        #if UsUart2
		else if (huart->Instance == USART2)
		{
			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_2(&(Uart_DmaRcv_Buffer[Huart2_Index][0]), len);
				}

				User_UART_IDLECallback_2();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收

			User_UART_Begin_DMAReceive(&huart2, &(Uart_DmaRcv_Buffer[Huart2_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart2_Index] = 0;

			Uart_PackCpt_Sign[Huart2_Index] = 1;
		}
		#endif
        else if (huart->Instance == USART3)
		{

			i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] > 0)
				{
					// 接收到数据处理函数
					User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{
					User_UART_RxCpltCallback_3(&(Uart_DmaRcv_Buffer[Huart3_Index][0]), len);
				}

				User_UART_IDLECallback_3();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收

			User_UART_Begin_DMAReceive(&huart3, &(Uart_DmaRcv_Buffer[Huart3_Index][0]), UART_RX_BUF_SIZE);

			Uart_DmaRcv_HalfCpt_Sign[Huart3_Index] = 0;

			Uart_PackCpt_Sign[Huart3_Index] = 1;
		}
		/*else if (huart->Instance == UART4)
		{

i = huart->Instance->ISR;
			i = huart->Instance->RDR;
			i = __HAL_DMA_GET_COUNTER(huart->hdmarx); // 剩余，有可能为0

			HAL_UART_DMAStopRx(huart);

			// 接收到的数据长度
			len = UART_RX_BUF_SIZE - i;

			if ((len > 0) && (len < UART_RX_BUF_SIZE)) // 有数据
			{
				if (Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] > 0)
				{

					// 接收到数据处理函数
					User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][UART_RX_BUF_SIZE / 2]),
											   len - UART_RX_BUF_SIZE / 2);
				}
				else
				{

					User_UART_RxCpltCallback_4(&(Uart_DmaRcv_Buffer[Huart4_Index][0]), len);
				}

				User_UART_IDLECallback_4();
			}
			else
			{
				__HAL_UART_CLEAR_OREFLAG(huart);
			}

			// 开始接收
			User_UART_Begin_DMAReceive(&huart4, &(Uart_DmaRcv_Buffer[Huart4_Index][0]), UART_RX_BUF_SIZE);
			Uart_DmaRcv_HalfCpt_Sign[Huart4_Index] = 0;
			Uart_PackCpt_Sign[Huart4_Index] = 1;
		} */
	}

	// 清除掉各种中断源
	//	if (huart->Instance == USART2)
	//		HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
	//	else if (huart->Instance == USART3)
	//		HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
	//	else if (huart->Instance == UART4)
	//		HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
}

/** @addtogroup  发送数据
 * @{
 */

void Uart1_Write(uint8_t *pSrc, uint8_t size)
{
	HAL_UART_Transmit_DMA(&huart1, pSrc, size);
}

void Uart2_Write(uint8_t *pSrc, uint8_t size)
{
    #if UsUart2
	// HAL_UART_Transmit_DMA(&huart2, pSrc, size);
	HAL_UART_Transmit(&huart2, pSrc, size, 100000);
    #endif
}
void Uart3_Write(uint8_t *pSrc, uint8_t size)
{
	// RS485DIR_Tx;
	// HAL_UART_Transmit_DMA(&huart3, pSrc, size);
	HAL_UART_Transmit(&huart3, pSrc, size, 100000);
	// RS485DIR_Rx;
}

void Uart4_Write(uint8_t *pSrc, uint8_t size)
{
	// HAL_UART_Transmit_DMA(&huart4, pSrc, size);
	//  HAL_UART_Transmit(&huart4, pSrc, size, 100000);
}

/**
 * @brief  判断是否收到一个包，即来过一次timeout中断
 * @param  index     串口序号
 * @return uint8_t
 */
uint8_t bCheckPackArrived(uint8_t index)
{
	return Uart_PackCpt_Sign[index];
}

/**
 * @brief  清除收到一个包标志
 * @param  index     串口序号
 * @return uint8_t
 */
void ResetPackArrivedSign(uint8_t index)
{
	Uart_PackCpt_Sign[index] = 0;
}

/**
 * @}
 */

/* USER CODE END 1 */

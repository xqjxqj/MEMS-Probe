#include "main.h"
#include "bldc.h"

extern CAN_HandleTypeDef hcan1;

#define CanHandle hcan1

CanTxMsg TxMessage;
CanRxMsg RxMessage;

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;

uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
static void CAN_Config(void)
{
#if 0
	// GPIO_InitTypeDef  GPIO_InitStructure;
	// CAN_InitTypeDef        CAN_InitStructure;
	// CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	// /* CAN GPIOs configuration **************************************************/

	// /* Enable GPIO clock */
	// RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

	// /* Connect CAN pins to AF9 */
	// GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	// GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);

	// /* Configure CAN RX and TX pins */
	// GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	// GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	// /* CAN configuration ********************************************************/
	// /* Enable CAN clock */
	// RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	// /* CAN register init */
	// CAN_DeInit(CANx);

	// /* CAN cell init */
	// CAN_InitStructure.CAN_TTCM = DISABLE;
	// CAN_InitStructure.CAN_ABOM = DISABLE;
	// CAN_InitStructure.CAN_AWUM = DISABLE;
	// CAN_InitStructure.CAN_NART = DISABLE;
	// CAN_InitStructure.CAN_RFLM = DISABLE;
	// CAN_InitStructure.CAN_TXFP = DISABLE;
	// CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	// CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	// /* CAN Baudrate = 1 MBps (CAN clocked at 30 MHz) */
	// //CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
	// //CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	// //CAN_InitStructure.CAN_Prescaler = 2;

	// /* CAN Baudrate = 125kBps (CAN clocked at 24 MHz) */
	// // baudrate = APB1 clock / (CAN_BS1+CAN_BS2+1)/CAN_Prescaler
	// CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
	// CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	// CAN_InitStructure.CAN_Prescaler = 16;

	// CAN_Init(CANx, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	///* Transmit Structure preparation */
	//TxMessage.StdId = 0x321;
	//TxMessage.ExtId = 0x01;
	//TxMessage.RTR = CAN_RTR_DATA;
	//TxMessage.IDE = CAN_ID_STD;
	//TxMessage.DLC = 1;

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);

#else
	CAN_FilterTypeDef sFilterConfig;

	/*##-2- Configure the CAN Filter ###########################################*/
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
	{
		/* Filter configuration Error */
		// Error_Handler();
	}

	/*##-3- Start the CAN peripheral ###########################################*/
	if (HAL_CAN_Start(&CanHandle) != HAL_OK)
	{
		/* Start Error */
		//Error_Handler();
	}

	/*##-4- Activate CAN RX notification #######################################*/
	if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		/* Notification Error */
		// Error_Handler();
	}

	TxHeader.StdId = 0x321;
	TxHeader.ExtId = 0x01;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 1;
	TxHeader.TransmitGlobalTime = DISABLE;

#endif
}

/**
  * @brief  Initializes the Rx Message.
  * @param  RxMessage: pointer to the message to initialize
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
	uint8_t ubCounter = 0;

	RxMessage->StdId = 0x00;
	RxMessage->ExtId = 0x00;
	RxMessage->IDE = CAN_ID_STD;
	RxMessage->DLC = 0;
	RxMessage->FMI = 0;
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		RxMessage->Data[ubCounter] = 0x00;
	}
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t i;

	/* Get RX message */
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		return;
	}

	RxMessage.StdId = RxHeader.StdId;
	RxMessage.ExtId = RxHeader.ExtId;
	RxMessage.IDE = RxHeader.IDE;
	RxMessage.RTR = RxHeader.RTR;
	RxMessage.DLC = RxHeader.DLC;
	for (i = 0; i < RxHeader.DLC; i++)
		RxMessage.Data[i] = RxData[i];
	
	BLDC_Rcv_Decode();
}

//
void CAN_User_Init(void)
{
	CAN_Config();

	Init_RxMes(&RxMessage);
}

// CAN���߷���
unsigned char CAN_WR_Bytes(unsigned char *buf, unsigned char size)
{
	unsigned char i, len;

	if (size > 8)
		len = 8;
	else
		len = size;

#if 0
	for (i = 0;i < len;i++)
		TxMessage.Data[i] = buf[i];
	//	for(;i<8;i++)
	//		TxMessage.Data[i]=0;

	TxMessage.DLC = len;

	return CAN_Transmit(CANx, &TxMessage);
#else
	// CAN_TxHeaderTypeDef TxHeader;
	// uint8_t TxData[8];
	// uint32_t TxMailbox;

	TxHeader.StdId = 0x11;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = len;
	TxHeader.TransmitGlobalTime = DISABLE;

	for (i = 0; i < len; i++)
		TxData[i] = buf[i];

	//   TxData[0] = 0xCA;
	//   TxData[1] = 0xFE;

	/* Request transmission */
	return (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox));

#endif
}

unsigned char CAN_WR_Packet(CanTxMsg *Tx)
{
	TxHeader.StdId = Tx->StdId;
	TxHeader.RTR = Tx->RTR;
	TxHeader.IDE = Tx->IDE;
	TxHeader.DLC = Tx->DLC;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Request transmission */
	return (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, Tx->Data, &TxMailbox));
}

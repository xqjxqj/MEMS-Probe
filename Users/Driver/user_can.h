
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __user_CAN_H_
#define __user_CAN_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  CAN Tx message structure definition  
  */
typedef struct
{
    uint32_t StdId; /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId; /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t IDE; /*!< Specifies the type of identifier for the message that 
                        will be transmitted. This parameter can be a value 
                        of @ref CAN_identifier_type */

    uint8_t RTR; /*!< Specifies the type of frame for the message that will 
                        be transmitted. This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

    uint8_t DLC; /*!< Specifies the length of the frame that will be 
                        transmitted. This parameter can be a value between 
                        0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
                        to 0xFF. */
} CanTxMsg;

/** 
  * @brief  CAN Rx message structure definition  
  */
typedef struct
{
    uint32_t StdId; /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId; /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t IDE; /*!< Specifies the type of identifier for the message that 
                        will be received. This parameter can be a value of 
                        @ref CAN_identifier_type */

    uint8_t RTR; /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of 
                        @ref CAN_remote_transmission_request */

    uint8_t DLC; /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to 
                        0xFF. */

    uint8_t FMI; /*!< Specifies the index of the filter the message stored in 
                        the mailbox passes through. This parameter can be a 
                        value between 0 to 0xFF */
} CanRxMsg;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;

/* Exported functions ------------------------------------------------------- */

extern void CAN_User_Init(void);
// extern unsigned char CAN_WR_Bytes(unsigned char *buf,unsigned char size);
extern unsigned char CAN_WR_Packet(CanTxMsg *Tx);

#endif /* __CAN_H */

/*****************************END OF FILE****/

/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-03-10
 * @copyright Copyright (c) 2022  CAS
 */
#ifndef __QBUSDrv_
#define __QBUSDrv_

/* Includes ------------------------------------------------------------------*/

/* Public define --------------------------------------------------------—----*/

/* Typedef ----------------------------------------------------------————————-*/
#define QBUSID_MIN 0
#define QBUSID_MAX 99

typedef enum
{
    MPRx05 = 0x05, // Surface Receiver (MPRx05)
    FSPPC,         // Field Service Primary PC
    FSPSC,         // Field Service Secondary PC
    DRT08,         // Reserved for Driller’s Remote Terminal (DRT 08)
    DRT09,         // Reserved for Driller’s Remote Terminal (DRT 09)
    MPTx20 = 20,   // Downhole Tool (MPTx20)
                   //  21-29  Reserved for downhole sub-systemnodes
    Undefine = 30  //  30-99 Undefined
} QBUS_ID_Type;

typedef enum
{
    ASCII_SOH = 0x01, // 标题开始
    ASCII_STX = 0x02, // 本文开始
    ASCII_ETX = 0x03, // 本文结束
    ASCII_EOT = 0x04, // end-of-transmission 传输结束
    ASCII_ENQ = 0x05, // 请求
    ASCII_ACK = 0x06, // 确认回应

    ASCII_LF = 0x0A,
    ASCII_CR = 0x0D,

    ASCII_NAK = 0x15, // 确认失败回应

    ASCII_WC = 0x23, // #  character is a wildcard and represents all characters 0-9.
    ASCII_FS = 0x2F, //   / forward slash 正斜杠
    ASCII_LA = 0x3C, // <   LeftArrow
    ASCII_RA = 0x3E, // >   RightArrow
} ASCII_CHAR_TYPE;

typedef enum
{
    QBUS_Message_Message = 0x01,   // 消息  以0x01开头
    QBUS_Message_Broadcast = 0x23, // 广播，含有 '>' 和  '#'  如 0A  0D  32  23  3E  05
    QBUS_Message_Polling = 0x3C,   // 轮询   '<' ，  0A  0D  32  31  3C  05
    QBUS_Message_Calling = 0x3E,   // 呼叫   '>' ，  0A  0D  32  31  3E   05
    QBUS_Message_Undefine = 0xFE
} QBUS_Message_Primary_TYPE;

typedef enum
{
    MTST_Reply = 0x3C,    //  /< Designating a reply (back)
    MTST_Download = 0x3E, // /> Designating a download
    MTST_Inquiry = 0x3F,  //   /? Designating an inquiry
} QBUS_MessageTransferSequence_TYPE;

typedef enum
{
    GV_TYPE_Int8 = 0, //
    GV_TYPE_Uint8,    //
    GV_TYPE_Int16,    //
    GV_TYPE_Uint16,   //
    GV_TYPE_Int32,    //
    GV_TYPE_Uint32,   //
    GV_TYPE_Float,    //
    GV_TYPE_Double    //
} QBUS_GV_TYPE;

typedef struct
{
    unsigned int Valid;
    float GVs[32];
    QBUS_GV_TYPE GV_Types[32];
} QBUS_Data_TYPE;

#define QBUS_MAX_GVs 16
#define default_QBUS_Clinet_ID 50

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
extern unsigned char QBUS_CallingID; // 刚收到的包的被呼叫节点编号
extern QBUS_Data_TYPE QBUS_Data;
extern  char QBUS_defaut_Client[];
extern  char QBUS_defaut_Server[];

/* Public functions ---------------------------------------------------------*/
// 产生主机发送的PollingSequence
unsigned char GenQBUS_PollingSequence(unsigned char targetID, unsigned char *retBytes);
QBUS_Message_Primary_TYPE QBUS_Received_One_Byte(unsigned char _rcvbyte);
unsigned short GenQBUS_ReplyPack(char *src, char *dst, char *content, unsigned char *retBytes);
unsigned short GenQBUS_GVs_Replay(unsigned char *retBytes);

#endif // __QBUSDrv_

/*********** (C) COPYRIGHT 2022 skyhigh *****END OF FILE****/

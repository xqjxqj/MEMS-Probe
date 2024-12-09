/**
 * @file qbus.c
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-03-10
 * @copyright Copyright (c) 2022  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
unsigned char _RxBytes[254];
unsigned short _RxBytesOffset;

// 保存刚收到完整QBUS内容
unsigned char Message_Header_Bytes[250], Message_Content_Bytes[250], Message_CRC_Bytes[8];

unsigned char QBUS_CallingID; // 刚收到的包的被呼叫节点编号
// char Message_Header_String[64], Message_Content_String[64], Message_CRC_String[64];
// char Message_Sender[64], Message_Receiver[64];

QBUS_MessageTransferSequence_TYPE Message_Header_Type;

unsigned char _TxBytes[128];
unsigned short _TxBytesLength;

unsigned short crc_result; // 计算crc
                           /* Public variables ---------------------------------------------------------*/
//提高Qbus的响应速度
QBUS_Data_TYPE QBUS_Data;

 char QBUS_defaut_Client[] = "SOFx51";
 char QBUS_defaut_Server[] = "MPTx20";

const unsigned short crc_coff[] =
    {
        0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
        0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
        0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
        0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
        0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
        0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
        0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
        0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
        0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
        0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
        0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
        0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
        0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
        0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
        0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
        0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
        0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
        0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
        0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
        0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
        0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
        0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
        0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
        0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
        0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
        0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
        0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
        0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
        0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
        0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
        0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
        0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  执行计算crc
 * @param  process_byte     My Param doc
 */
void calculate_crc(unsigned char process_byte)
{
    crc_result = ((crc_result >> 8) ^ crc_coff[((crc_result ^ process_byte) & 0xff)]);
}

unsigned short QBUS_CRC_Value(char *_headerbytes, char *_contentbytes)
{
    int i;
    crc_result = 0; //初始化CRC检验结果=0

    i = 0;
    while (_headerbytes[i] != 0)
    {
        calculate_crc(_headerbytes[i]); //调用检验计算函数
        //指向下一个潜在需要检验的字节
        i++;
    }

    i = 0;
    while (_contentbytes[i] != 0)
    {
        calculate_crc(_contentbytes[i]); //调用检验计算函数
        //指向下一个潜在需要检验的字节
        i++;
    }

    return crc_result;
}

/**
 * @brief 拼接_headerbytes和_headerbytes，计算crc
 *
 * @param _headerbytes
 * @param _contentbytes
 * @param pcrc
 */
void QBUS_CRC_String(char *_headerbytes, char *_contentbytes, char *pcrc)
{
    int i;
    crc_result = 0; //初始化CRC检验结果=0

    i = 0;
    while (_headerbytes[i] != 0)
    {
        calculate_crc(_headerbytes[i]); //调用检验计算函数
        //指向下一个潜在需要检验的字节
        i++;
    }

    i = 0;
    while (_contentbytes[i] != 0)
    {
        calculate_crc(_contentbytes[i]); //调用检验计算函数
        //指向下一个潜在需要检验的字节
        i++;
    }
    sprintf(pcrc, "%02X", crc_result);
}

unsigned char GetHexAscii(unsigned char hexval)
{
	unsigned char ret;
	
	hexval &= 0x0F;
	
	if( (hexval >=0 && hexval< 10) )
		return (hexval+0x30);
	else 
		return (hexval-10+'A');
}

/**
 * @brief  有了header和content后，计算crc
 *
 * @param header
 * @param content
 * @param pCrc
 * @return unsigned short
 */
unsigned short QBUS_CRC_Bytes(char *header, char *content, unsigned char *pCrc)
{
    crc_result = 0; //初始化CRC检验结果=0

    int m;
    for (m = 0; m < strlen(header); m++)
    {
        calculate_crc((unsigned char)header[m]); //调用检验计算函数
    }

    for (m = 0; m < strlen(content); m++)
    {
        calculate_crc((unsigned char)content[m]); //调用检验计算函数
    }

    pCrc[0] = (unsigned char)(crc_result >> 8);
    pCrc[1] = (unsigned char)(crc_result >> 0);

    return crc_result;
}

/**
 * @brief 获取某1个GV值
 *
 * @param str
 * @param index
 */
void Get_QBUS_OneGV(char *str, unsigned short index)
{
    QBUS_Data.GVs[index] = atof(str);
    QBUS_Data.Valid |= 0x01 << index;
}

/**
 * @brief 解析从机的数据中GV值
 *
 */
// MPTx20WRTi28/>GV0=88.00;GV1=000;GV2=000;GV3=046;GV4=000;GV5=0.0000;GV6=0.00;GV7=000
void Get_QBUS_GVs(char *str)
{
    char tmpstr[32];
    char tmpstrlen;
    unsigned char index;
    char tmpC;
    char *offset;

    offset = strstr((const char *)str, "V");
    if (*offset == 0)
        return;

    QBUS_Data.Valid = 0;

    offset++; //
    index = 0;
    while (*offset)
    {
        // 查找 GV 序号
        while (*offset)
        {
            if (*offset != '=')
            {
                if (((unsigned char)*offset >= 0x30) && ((unsigned char)*offset <= 0x39))
                {
                    index *= 10;
                    index += (unsigned char)(*offset - 0x30);
                }
                offset++;
            }
            else // =
            {
                offset++;

                tmpstrlen = 0;
                break;
            }
        }
        // 得到数据
        while (*offset)
        {
            if (*offset != ';' && *offset != (char)ASCII_ETX)
            {
                tmpstr[tmpstrlen++] = *offset;
                offset++;
            }
            else if (*offset == (char)ASCII_ETX)
            {
                tmpstr[tmpstrlen++] = 0;

                Get_QBUS_OneGV(tmpstr, index);
                index = 0;

                goto endcontent;
            }
            else if (*offset == ';')
            {
                tmpstr[tmpstrlen++] = 0;

                Get_QBUS_OneGV(tmpstr, index);
                index = 0;
                offset++;
                break;
            }
        }
    }

endcontent:
    return;
}

/**
 * @brief 解析收到的QBUS字符串
 *
 * @return QBUS_Message_Primary_TYPE
 */
QBUS_Message_Primary_TYPE Decode_Rx_Message(void)
{
    char rcv_crc[5];

    if (_RxBytes[0] == (unsigned char)ASCII_LF && _RxBytes[1] == (unsigned char)ASCII_CR)
    {
        if (_RxBytes[4] == (unsigned char)QBUS_Message_Polling) // 轮询   '<' ，  0A  0D  32  31  3C  05
        {
            QBUS_CallingID = (_RxBytes[2] - (unsigned char)0x30) * 10 + (_RxBytes[3] - (unsigned char)0x30);

            return QBUS_Message_Polling;
        }
        else if (_RxBytes[4] == (unsigned char)QBUS_Message_Calling) // 呼叫   '>' ，  0A  0D  32  31  3E   05
        {
            if (_RxBytes[3] == (unsigned char)QBUS_Message_Broadcast) // 广播，含有 '>' 和  '#'  如 0A  0D  32  23  3E  05
            {
                QBUS_CallingID = (_RxBytes[2] - (unsigned char)0x30) * 10;
                return QBUS_Message_Broadcast;
            }
            else // 呼叫   '>' ，  0A  0D  32  31  3E   05
            {
                QBUS_CallingID = (_RxBytes[2] - (unsigned char)0x30) * 10 + (_RxBytes[3] - (unsigned char)0x30);
                return QBUS_Message_Calling;
            }
        }
    }
    else if (_RxBytes[0] == (unsigned char)ASCII_SOH) // 消息  以0x01开头
    {
        int i = 1, m = 0;

        //   SOH  [header]  STX  [data]  ETX  [crc]  ENQ
        while (_RxBytes[i] != (unsigned char)ASCII_STX)
        {
            Message_Header_Bytes[m++] = _RxBytes[i++];
        }
        Message_Header_Bytes[m] = 0; // 头

        // Message_Header_String = Encoding.ASCII.GetString(Message_Header_Bytes, 0, m);
        // Message_Sender = Message_Header_String.Substring(0, 6);
        // Message_Receiver = Message_Header_String.Substring(6, 6);

        Message_Header_Type = (QBUS_MessageTransferSequence_TYPE)(Message_Header_Bytes[m - 1]);

        i++;
        m = 0;
        while (_RxBytes[i] != (unsigned char)ASCII_ETX)
        {
            Message_Content_Bytes[m++] = _RxBytes[i++];
        }
        // Message_Content_Bytes[m++] = ASCII_ETX; // 补充进去，后续好识别 内容
        Message_Content_Bytes[m] = 0; // 内容
        // Message_Content_String = Encoding.ASCII.GetString(Message_Content_Bytes, 0, m);

        i++;
        int n = 0;
        while (_RxBytes[i] != (unsigned char)ASCII_ENQ)
        {
            Message_CRC_Bytes[n++] = _RxBytes[i++];
        }
        Message_CRC_Bytes[n] = 0;

        // Message_CRC_String = Encoding.ASCII.GetString(Message_CRC_Bytes, 0, m);

        QBUS_CRC_String((char *)Message_Header_Bytes, (char *)Message_Content_Bytes, rcv_crc);

        // 对比crc结果
        if (rcv_crc[0] == (char)Message_CRC_Bytes[0] &&
            rcv_crc[1] == (char)Message_CRC_Bytes[1] &&
            rcv_crc[2] == (char)Message_CRC_Bytes[2] &&
            rcv_crc[3] == (char)Message_CRC_Bytes[3])
        {
            Message_Content_Bytes[m++] = ASCII_ETX; // 补充进去，后续好识别 内容
            Message_Content_Bytes[m] = 0;           // 内容

            Get_QBUS_GVs((char *)(Message_Content_Bytes));
            return QBUS_Message_Message;
        }
    }

    return QBUS_Message_Undefine;
}

/* Public functions ---------------------------------------------------------*/

/**
 * @brief  每次收到一个字节后处理
 *
 * @param _rcvbyte
 * @return QBUS_Message_Primary_TYPE
 */
QBUS_Message_Primary_TYPE QBUS_Received_One_Byte(unsigned char _rcvbyte)
{
    switch (_rcvbyte)
    {
    case ((unsigned char)ASCII_SOH): // 0x01,  // 标题开始
    {
        _RxBytesOffset = 0;

        break;
    }
    case ((unsigned char)ASCII_STX): // 0x02,  // 本文开始
    {

        break;
    }
    case ((unsigned char)ASCII_ETX): // 0x03, // 本文结束
    {

        break;
    }
    case ((unsigned char)ASCII_EOT): //  0x04, // end-of-transmission 传输结束
    {
        _RxBytesOffset = 0;
        break;
    }
    case ((unsigned char)ASCII_ENQ): // 0x05,// 请求
    {
        // 一包结束
        _RxBytes[_RxBytesOffset++] = _rcvbyte;
        return Decode_Rx_Message();

        break;
    }
    case ((unsigned char)ASCII_ACK): // 0x06,  // 确认回应
    {

        break;
    }
    case ((unsigned char)ASCII_LF): // 0x0A,
    {
        _RxBytesOffset = 0;
        break;
    }
    case ((unsigned char)ASCII_CR): // 0x0D
    {

        break;
    }
    }
    _RxBytes[_RxBytesOffset++] = _rcvbyte;
    return QBUS_Message_Undefine;
}

/**
 * @brief 主机 产生发送的PollingSequence
 *
 * @param targetID
 * @param retBytes
 * @return unsigned char
 */
unsigned char GenQBUS_PollingSequence(unsigned char targetID, unsigned char *retBytes)
{
    if (targetID > 99)
        targetID = 99;

    retBytes[0] = ASCII_LF;
    retBytes[1] = ASCII_CR;
    retBytes[2] = 0x30 + (unsigned char)(targetID / 10);
    retBytes[3] = 0x30 + (unsigned char)(targetID % 10);
    retBytes[4] = QBUS_Message_Polling;
    retBytes[5] = ASCII_ENQ;

    return 0x06;
}

/**
 * @brief 从机   拼接回复包
 *
 * @param src
 * @param dst
 * @param content
 * @param retBytes
 * @return unsigned short
 */
unsigned short GenQBUS_ReplyPack(char *src, char *dst, char *content, unsigned char *retBytes)
{
    char header[254];
    int offset = 0;
    unsigned char tmpBytes[2];
    unsigned char m;
	unsigned short _crc;
    // byte[] retBytes = new byte[22 + content.Length];   // header.length=14 , ascii control 4 crc 4

    // header = dst + src + "/<";
    sprintf(header, "%s%s/>", dst, src);

    retBytes[offset++] = (unsigned char)ASCII_SOH; //   01H

    // header
    // tmpBytes = Encoding.ASCII.GetBytes(header);
    for (int i = 0; i < strlen(header); i++)
        retBytes[offset++] = (unsigned char)header[i];

    retBytes[offset++] = (unsigned char)ASCII_STX; //   02H

    // data
    // tmpBytes = Encoding.ASCII.GetBytes(content);
    for (int i = 0; i < strlen(content); i++)
        retBytes[offset++] = (unsigned char)content[i];

    retBytes[offset++] = (unsigned char)ASCII_ETX; //   03H

    // crc
    _crc = QBUS_CRC_Bytes(header, content, tmpBytes);
	retBytes[offset++]=	GetHexAscii((unsigned char)( _crc >> 12 ) );
	retBytes[offset++]=	GetHexAscii((unsigned char)( _crc >> 8 ) );
	retBytes[offset++]=	GetHexAscii((unsigned char)( _crc >> 4 ) );
	retBytes[offset++]=	GetHexAscii((unsigned char)( _crc >> 0 ) );

    retBytes[offset++] = (unsigned char)ASCII_ENQ; //   05H

    return offset;
}

/**
 * @brief  把QBUS_Data中的数据，组包成QBUS回复包
 *
 * @return unsigned short
 */
unsigned short GenQBUS_GVs_Replay(unsigned char *retBytes)
{
    int i, j, m;
    unsigned short MASK = 0x01;
    unsigned char index = 0;
    char content[240];
    int content_index = 0;
    char tmpc[64];

    // 得到content
    for (i = 0; i < QBUS_MAX_GVs; i++)
    {
        if ((QBUS_Data.Valid & MASK) == MASK)
        {
            switch (QBUS_Data.GV_Types[i])
            {
            case GV_TYPE_Int8:
            case GV_TYPE_Uint8:
            case GV_TYPE_Int16:
            case GV_TYPE_Uint16:
            case GV_TYPE_Int32:
            case GV_TYPE_Uint32:
                // 得到单条
                m = sprintf(tmpc, "GV%d=%d;", i,(int32_t)QBUS_Data.GVs[i]);
                for (j = 0; j < m; j++)
                    content[content_index++] = tmpc[j];
                content[content_index + 1] = 0;

                break;
            case GV_TYPE_Float:
            case GV_TYPE_Double:

                // 得到单条
                m = sprintf(tmpc, "GV%d=%.1f;", i,QBUS_Data.GVs[i]);
                for (j = 0; j < m; j++)
                    content[content_index++] = tmpc[j];
                content[content_index + 1] = 0;

                break;
            default:
                break;
            }
        }
    }
	
	content[content_index ] = 0; // 去掉最后一个;
	
    return GenQBUS_ReplyPack((char *)QBUS_defaut_Client, (char *)QBUS_defaut_Server, content, retBytes);
}

/*********** (C) COPYRIGHT 2022  skyhigh  *****END OF FILE****/

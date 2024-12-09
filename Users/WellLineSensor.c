/**
 * @file WellLineSensor
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-05-15
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

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

unsigned short CRC_U16;

//-------- 接收数据用 -------
// 存储 sensor 发送的字节流
unsigned char WellLine_DLCmd_Packet_Buf[128];
unsigned char WellLineSensor_DLCmd_Packet_Buf_Offset = 0;
// 提示收到一个完整包
unsigned char bGot_MWDReportPackect = false; //

unsigned char LastDLCmd;             // 上一次接收到的命令
unsigned char LastDLCmd_Payload[32]; // 上一次接收到的附加内容

/* Public variables ---------------------------------------------------------*/

Identity_Typedef MyID;
unsigned int DeviceID;

//---------------------日期 时间---------------------------
// 月份 Jan  Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
const char str_builddate[12] = __DATE__; // 示例 "Mar 23 2022"
const char str_buildtime[10] = __TIME__; //     11:51:32

DatetimeStruct_TypeDef BuildDatetime;

char Str_BuildDatetime[13]; //"230613110532" 2023-06-13 11:05:32

char Str_ID[49];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

const unsigned short crc16tab[256] = {0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
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

const unsigned int crc32tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL};

		
//这段代码是CRC-16（循环冗余校验）算法中的一个步骤，用于更新一个16位的CRC值
//具体来说，它是使用查表法（table-driven approach）来加速CRC的计算。
void CalcCRC16(unsigned char c)
{
    CRC_U16 = ((unsigned short)((CRC_U16 >> 8) ^ crc16tab[(CRC_U16 ^ c) & 0xFF]));
}

//用于计算给定数据缓冲区buf的CRC-32校验值
unsigned int CalcCRC32(unsigned char *buf, unsigned int size)
{
    unsigned int i, crc;
    crc = 0xFFFFFFFF;

    for (i = 0; i < size; i++)
        crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFF;
}

void DLCmd_Report_Buffer_Reset()
{
    int i;
    for (i = 0; i < sizeof(WellLine_DLCmd_Packet_Buf); i++)
        WellLine_DLCmd_Packet_Buf[i] = 0;
    ;
    WellLineSensor_DLCmd_Packet_Buf_Offset = 0;
}

/* Public functions ---------------------------------------------------------*/

/**
 * @brief 得到版本
 *
 */

//获取时间并生成文件名
void Get_BuildDatetime(void)
{
#if 0
    unsigned char year, month;
    unsigned char d1, d2;

    // 得到编译时间
    // 日 str_date[4] str_date[5]
    d1 = str_builddate[4] - '0';      // 日期 十位
    d2 = str_builddate[5] - '0';      // 日期 个位
    BuildDatetime.dat = d1 * 10 + d2; //

    // 月份字符串转换成 1~C：先判断第3个字母，再判第2个字母，得到对应月份
    // str_date[0] str_date[1] str_date[2]
    switch (str_builddate[2])
    {
    case 'n':                                    // Jan和Jun
        month = str_builddate[1] == 'a' ? 1 : 6; // 判断1月还是6月
        break;

    case 'b': // Feb
        month = 2;
        break;

    case 'r':                                    // Mart和Apr
        month = str_builddate[1] == 'a' ? 3 : 4; // 判断3月还是4月
        break;

    case 'y': // May
        month = 5;
        break;

    case 'l': // Jul
        month = 7;
        break;

    case 'g': // Aug
        month = 8;
        break;

    case 'p': // Sep
        month = 9;
        break;

    case 't':         // Oct
        month = 0x0A; // 10
        break;

    case 'v':         // Nov
        month = 0x0B; // 11
        break;

    case 'c':         // Dec
        month = 0x0C; // 12
        break;
    }
    BuildDatetime.month = month;

    // str_date[7] str_date[8] str_date[9] str_date[10]
    BuildDatetime.year = (str_builddate[9] - '0') * 10 + (str_builddate[10] - '0'); // 年份

    BuildDatetime.hour = (str_buildtime[0] - '0') * 10 + (str_buildtime[1] - '0');
    BuildDatetime.minute = (str_buildtime[3] - '0') * 10 + (str_buildtime[4] - '0');
    BuildDatetime.second = (str_buildtime[6] - '0') * 10 + (str_buildtime[7] - '0');

#else
    // year
    Str_BuildDatetime[0] = str_builddate[9];
    Str_BuildDatetime[1] = str_builddate[10];

    // month
    switch (str_builddate[2])
    {
    case 'n': // Jan和Jun
        // 判断1月还是6月
        if (str_builddate[1] == 'a')
        {
            Str_BuildDatetime[2] = '0';
            Str_BuildDatetime[3] = '1';
        }
        else
        {
            Str_BuildDatetime[2] = '0';
            Str_BuildDatetime[3] = '6';
        }
        break;
    case 'b': // Feb
        Str_BuildDatetime[2] = '0';
        Str_BuildDatetime[3] = '2';
        break;
    case 'r': // Mart和Apr
        if (str_builddate[1] == 'a')
        {
            Str_BuildDatetime[2] = '0';
            Str_BuildDatetime[3] = '3';
        }
        else
        {
            Str_BuildDatetime[2] = '0';
            Str_BuildDatetime[3] = '4';
        }
        break;
    case 'y': // May
        Str_BuildDatetime[2] = '0';
        Str_BuildDatetime[3] = '5';
        break;
    case 'l': // Jul
        Str_BuildDatetime[2] = '0';
        Str_BuildDatetime[3] = '7';
        break;
    case 'g': // Aug
        Str_BuildDatetime[2] = '0';
        Str_BuildDatetime[3] = '8';
        break;
    case 'p': // Sep
        Str_BuildDatetime[2] = '0';
        Str_BuildDatetime[3] = '9';
        break;
    case 't': // Oct
        Str_BuildDatetime[2] = '1';
        Str_BuildDatetime[3] = '0';
        break;
    case 'v': // Nov
        Str_BuildDatetime[2] = '1';
        Str_BuildDatetime[3] = '1';
        break;
    case 'c': // Dec
        Str_BuildDatetime[2] = '1';
        Str_BuildDatetime[3] = '2';
        break;
    }

    // dat
    Str_BuildDatetime[4] = str_builddate[4];
    Str_BuildDatetime[5] = str_builddate[5];

    // hour
    Str_BuildDatetime[6] = str_buildtime[0];
    Str_BuildDatetime[7] = str_buildtime[1];

    // min
    Str_BuildDatetime[8] = str_buildtime[3];
    Str_BuildDatetime[9] = str_buildtime[4];

    // sec
    Str_BuildDatetime[10] = str_buildtime[6];
    Str_BuildDatetime[11] = str_buildtime[7];

    Str_BuildDatetime[12] = 0;
#endif
}

void Init_ID(void)
{
    // <41><50><53><3A><53><2F><4E><20> prints the message APS:S/N .
    // <Serial Number> is the system serial number.
    // <CR><LF> is carriage return and line feed.
    // <56><05><72><3A><20> prints the message VER:.
    // <Firmware version> is the directional sensor firmware version.
    // <CR><LF> is carriage return and line feed.

    //    char SN[8];
    //     char SerialNumber[16];
    //     char Ver[5];
    //     char Firmware[16];
    // <06><00><41><50><53><3A><53><2F><4E><20><Serial Number><CR><LF><56><05><72><3A><20><FirmwareVersion><CR><LF><CRC16><06><04>

    DeviceID = CalcCRC32(U_ID, sizeof(U_ID)); // 8 characters

    Get_BuildDatetime();

    switch (SN_Version)
    {
    case (QFlex200_Mag614_Probe_V01): //  =0x10,  // 使用QFlex185石英加速度+Mag614磁通门  WL615
    {
        sprintf(MyID.SN, "%s-%02X-%08X", "WL615", HW_Version, DeviceID); // char SN[17];
        break;
    }
    case (QFlex185_IGGMag_Probe_V01): //  =0x20,  // 使用QFlex185石英加速度+IGG自研磁通门  WL616
    {
        sprintf(MyID.SN, "%s-%02X-%08X", "WL616", HW_Version, DeviceID); // char SN[17];
        break;
    }
    case (MEMSs_HMC102x_Probe_V01): //  =0x30,    // 使用多组MEMS加速度+HMC1021/2磁阻     WL618
    {
        sprintf(MyID.SN, "%s-%02X-%08X", "WL618", HW_Version, DeviceID); // char SN[17];
        break;
    }
    case (MEMSs_RM3100_Probe_V01): //  =0x40 // 使用单组组MEMS加速度+RM3100磁     WL315
    {
        sprintf(MyID.SN, "%s-%02X-%08X", "WL315", HW_Version, DeviceID); // char SN[17];
        break;
    }
    }

    sprintf(MyID.Ver, "%02X-%s", FW_Version, Str_BuildDatetime); // 01-230613152950 char Ver[16];

    // IGG:S/N WL616-01-ABCDEFGH\r\n
    // VER:01-230613152950\r\n
    // 8+8+9+2+4+3+12+2=48
    sprintf(Str_ID, "IGG:S/N %s\r\nVER:%s\r\n", MyID.SN, MyID.Ver);
	//sprintf(Str_ID, "IGG:S/N %s\r\n", MyID.SN);
	//sprintf((char *)(&(Str_ID[27])), "VER:%s\r\n",MyID.Ver);
    Str_ID[sizeof(Str_ID) - 1] = 0;
}

/**
 * @brief  处理接收到的命令
 *
 * @param _rcvbyte
 * @return unsigned char
 */
unsigned char Receive_Byte(unsigned char _rcvbyte)
{
    int _sta = 0;
    unsigned char i;
    unsigned char cmd;

    WellLine_DLCmd_Packet_Buf[WellLineSensor_DLCmd_Packet_Buf_Offset++] = _rcvbyte;

    if (WellLineSensor_DLCmd_Packet_Buf_Offset >= sizeof(WellLine_DLCmd_Packet_Buf)) // 防止越界
    {
        WellLineSensor_DLCmd_Packet_Buf_Offset = 0;
    }

    if (WellLineSensor_DLCmd_Packet_Buf_Offset >= 4) // 略过包头，检查数据和CRC部分的连续0x06 0x06
    {
        // 在数据域和CRC中出现了0x06 0x06，则改成0x06
        if ((WellLine_DLCmd_Packet_Buf[WellLineSensor_DLCmd_Packet_Buf_Offset - 2] == DLCmd_Pack_TOKEN) &&
            (WellLine_DLCmd_Packet_Buf[WellLineSensor_DLCmd_Packet_Buf_Offset - 1] == DLCmd_Pack_TOKEN))
        {
            WellLineSensor_DLCmd_Packet_Buf_Offset--;
            return DLCmd_PackType_CMD_Undefine;
        }
    }

    if (WellLineSensor_DLCmd_Packet_Buf_Offset >= 6) // 最小包长也是6
    {
        if ((WellLine_DLCmd_Packet_Buf[WellLineSensor_DLCmd_Packet_Buf_Offset - 2] == DLCmd_Pack_TOKEN) &&
            (WellLine_DLCmd_Packet_Buf[WellLineSensor_DLCmd_Packet_Buf_Offset - 1] == DLCmd_Pack_EOT)) // 刚结束一包
        {
            // 解析
            // 先找到包开始位置，包起始字节总是Pack_TOKEN(06)
            _sta = 0;
            while (WellLine_DLCmd_Packet_Buf[_sta] != DLCmd_Pack_TOKEN && _sta < WellLineSensor_DLCmd_Packet_Buf_Offset)
            {
                _sta++;
            }

            _sta++;
             cmd = WellLine_DLCmd_Packet_Buf[_sta];
            switch (cmd)
            {
            case (DLCmd_PackType_RealtimeData):
            {
                _sta++;
                LastDLCmd = DLCmd_PackType_RealtimeData;
                LastDLCmd_Payload[0] = WellLine_DLCmd_Packet_Buf[_sta];
                break;
            }
            case (DLCmd_PackType_SetRTC): //
            {
                _sta++;

                LastDLCmd = DLCmd_PackType_SetRTC;
                for (i = 0; i < 6; i++)
                    LastDLCmd_Payload[i] = WellLine_DLCmd_Packet_Buf[_sta++];

                break;
            }
            case (DLCmd_PackType_USB_Storage): //
            {
                _sta++;
                LastDLCmd = DLCmd_PackType_USB_Storage;
                LastDLCmd_Payload[0] = WellLine_DLCmd_Packet_Buf[_sta];

                break;
            }
            case (DLCmd_PackType_GetRTC): //
            {

                LastDLCmd = DLCmd_PackType_GetRTC;
                break;
            }
            case (DLCmd_PackType_Get_File_Infor): //
            {
                LastDLCmd = DLCmd_PackType_Get_File_Infor;

                break;
            }
            case (DLCmd_PackType_Get_File_Data): //
            {
                LastDLCmd = DLCmd_PackType_Get_File_Data;

                break;
            }
            case (DLCmd_PackType_Del_File): //
            {
                LastDLCmd = DLCmd_PackType_Del_File;
                break;
            }  
            default:
            {
                LastDLCmd = cmd; // DLCmd_PackType_CMD_Undefine;
                break;
            }
            }

            DLCmd_Report_Buffer_Reset();
            return LastDLCmd; // 收到正确包
        }
    }

    // 未收到正确包
    return DLCmd_PackType_CMD_Undefine;
}

/**
 * @brief  组建ID包
 * @param  pRetBytes
 * @return unsigned short
 */
unsigned short Gen_Tx_Pack_Id(unsigned char *pRetBytes)
{
    // <06><00><41><50><53><3A><53><2F><4E><20><Serial Number><CR><LF><56><05><72><3A><20><FirmwareVersion><CR><LF><CRC16><06><04>
    // <06> is the token.
    // <00> identifies directional sensor identity packet.
    // <41><50><53><3A><53><2F><4E><20> prints the message IGG:S/N .
    // <Serial Number> is the system serial number.
    // <CR><LF> is carriage return and line feed.
    // <56><05><72><3A><20> prints the message VER:.
    // <Firmware version> is the directional sensor firmware version.
    // <CR><LF> is carriage return and line feed.
    // <CRC> is the 16-bit cyclic redundancy check.
    // <06> is the token.
    // <04> is EOT.

    unsigned short offset = 0;
    unsigned char tmp, i;
    // char tmpC[48];

    // IGG:S/N WL616-01-ABCDEFGH\r\n
    // VER:01-230613152950\r\n
    // 8+8+8+2+4+15+2=47
    // sprintf(tmpC, "IGG:S/N %s\r\nVER:%s\r\n", MyID.SN, MyID.Ver);

    CRC_U16 = 0;

    // Sensor_Pack_TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    CalcCRC16(Sensor_Pack_TOKEN);

    // pack type
    pRetBytes[offset++] = Sensor_PackType_Identity;
    CalcCRC16(Sensor_PackType_Identity);

    //
    for (i = 0; i < sizeof(Str_ID) - 1; i++)
    {
        pRetBytes[offset++] = Str_ID[i];
        CalcCRC16(Str_ID[i]);
        if (Str_ID[i] == Sensor_Pack_TOKEN)
        {
            pRetBytes[offset++] = Sensor_Pack_TOKEN;
            CalcCRC16(Sensor_Pack_TOKEN);
        }
    }

    // crc  // LSB first
    tmp = (unsigned char)(CRC_U16);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    tmp = (unsigned char)(CRC_U16 >> 8);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }
    // TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    // EOT
    pRetBytes[offset++] = Sensor_Pack_EOT;

    return offset;
}

/**
 * @brief  构建 发送 原始数据包
 *
 * @param _pRawADC   存储rawadc数据的首地址，数据为16bit
 * @param pRetBytes  数据包，字节流
 * @return unsigned short 数据包字节数
 */
unsigned short Gen_Tx_Pack_RawADC(unsigned short *_pRawADC, unsigned char *pRetBytes)
{
    unsigned short offset = 0;
    unsigned char ch, tmp;

    CRC_U16 = 0;

    // Sensor_Pack_TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    CalcCRC16(Sensor_Pack_TOKEN);

    // pack type
    pRetBytes[offset++] = Sensor_PackType_RawADC;
    CalcCRC16(Sensor_PackType_RawADC);

    // raw adc  // MSB first
   // for (ch = 0; ch < ExtADC_Chs + IntADC_VT_Chs; ch++)
	for (ch = 0; ch < 24; ch++)
    {
        tmp = (unsigned char)(_pRawADC[ch]);
        pRetBytes[offset++] = tmp;
        CalcCRC16(tmp);

        if (tmp == Sensor_Pack_TOKEN)
        {
            pRetBytes[offset++] = Sensor_Pack_TOKEN;
            // CalcCRC16(Sensor_Pack_TOKEN);
        }

        tmp = (unsigned char)(_pRawADC[ch] >> 8);
        pRetBytes[offset++] = tmp;
        CalcCRC16(tmp);

        if (tmp == Sensor_Pack_TOKEN)
        {
            pRetBytes[offset++] = Sensor_Pack_TOKEN;
            // CalcCRC16(Sensor_Pack_TOKEN);
        }
    }

    // crc  // LSB first
    tmp = (unsigned char)(CRC_U16);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    tmp = (unsigned char)(CRC_U16 >> 8);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    // TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;

    // EOT
    pRetBytes[offset++] = Sensor_Pack_EOT;

    return offset;
}

/**
 * @brief 构建 发送 RTC
 *
 * @param _rtc   依次 yy MM DD HH mm ss
 * @param pRetBytes 数据包，字节流
 * @return unsigned short  数据包字节数
 */
unsigned short Gen_Tx_Pack_RTC(unsigned char *_rtc, unsigned char *pRetBytes)
{
    unsigned short offset = 0;
    unsigned char tmp, i;

    CRC_U16 = 0;

    // Sensor_Pack_TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    CalcCRC16(Sensor_Pack_TOKEN);

    // pack type
    pRetBytes[offset++] = Sensor_PackType_RTC;
    CalcCRC16(Sensor_PackType_RTC);

    // _rtc
    for (i = 0; i < 6; i++)
    {
        tmp = *_rtc++;
        pRetBytes[offset++] = tmp;
        CalcCRC16(tmp);
        if (tmp == Sensor_Pack_TOKEN)
        {
            pRetBytes[offset++] = Sensor_Pack_TOKEN;
            // CalcCRC16(Sensor_Pack_TOKEN);
        }
    }

    // crc  // LSB first
    tmp = (unsigned char)(CRC_U16);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    tmp = (unsigned char)(CRC_U16 >> 8);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }
    // TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    // EOT
    pRetBytes[offset++] = Sensor_Pack_EOT;

    return offset;
}

/**
 * @brief 构建 发送 存储文件信息
 *
 * @param filesize  数据文件大小
 * @param pRetBytes 数据包，字节流
 * @return unsigned short  数据包字节数
 */
unsigned short Gen_Tx_Pack_FileInfor(unsigned int filesize, unsigned char *pRetBytes)
{
    unsigned short offset = 0;
    unsigned char tmp;

    CRC_U16 = 0;

    // Sensor_Pack_TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    CalcCRC16(Sensor_Pack_TOKEN);

    // pack type
    pRetBytes[offset++] = Sensor_PackType_File_Infor;
    CalcCRC16(Sensor_PackType_File_Infor);

    // fileinfor  // MSB first
    tmp = (unsigned char)(filesize >> 24);
    pRetBytes[offset++] = tmp;
    CalcCRC16(tmp);
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
        // CalcCRC16(Sensor_Pack_TOKEN);
    }
    tmp = (unsigned char)(filesize >> 16);
    pRetBytes[offset++] = tmp;
    CalcCRC16(tmp);
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
        // CalcCRC16(Sensor_Pack_TOKEN);
    }
    tmp = (unsigned char)(filesize >> 8);
    pRetBytes[offset++] = tmp;
    CalcCRC16(tmp);
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
        // CalcCRC16(Sensor_Pack_TOKEN);
    }
    tmp = (unsigned char)(filesize);
    pRetBytes[offset++] = tmp;
    CalcCRC16(tmp);
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
        // CalcCRC16(Sensor_Pack_TOKEN);
    }

    // crc  // LSB first
    tmp = (unsigned char)(CRC_U16);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    tmp = (unsigned char)(CRC_U16 >> 8);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    // TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    // EOT
    pRetBytes[offset++] = Sensor_Pack_EOT;

    return offset;
}

/**
 * @brief 发送存储文件的原始数据
 *
 * @param pfiledata  存储文件的原始数据首地址
 * @param size  存储文件的原始数据大小
 * @param pRetBytes  数据包字节流
 * @return unsigned short 数据包字节大小
 */
unsigned short Gen_Tx_Pack_FileData(unsigned char *pfiledata, unsigned short size, unsigned char *pRetBytes)
{
    unsigned short offset = 0;
    unsigned char tmp, i;

    CRC_U16 = 0;

    // Sensor_Pack_TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    CalcCRC16(Sensor_Pack_TOKEN);

    // pack type
    pRetBytes[offset++] = Sensor_PackType_RTC;
    CalcCRC16(Sensor_PackType_RTC);

    // filedata
    for (i = 0; i < size; i++)
    {
        tmp = *pfiledata++;
        pRetBytes[offset++] = tmp;
        CalcCRC16(tmp);
        if (tmp == Sensor_Pack_TOKEN)
        {
            pRetBytes[offset++] = Sensor_Pack_TOKEN;
            // CalcCRC16(Sensor_Pack_TOKEN);
        }
    }

    // crc  // LSB first
    tmp = (unsigned char)(CRC_U16);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    tmp = (unsigned char)(CRC_U16 >> 8);
    pRetBytes[offset++] = tmp;
    if (tmp == Sensor_Pack_TOKEN)
    {
        pRetBytes[offset++] = Sensor_Pack_TOKEN;
    }

    // TOKEN
    pRetBytes[offset++] = Sensor_Pack_TOKEN;
    // EOT
    pRetBytes[offset++] = Sensor_Pack_EOT;

    return offset;
}

/*********** (C) COPYRIGHT 2022  skyhigh  *****END OF FILE****/

/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-05-15
 * @copyright Copyright (c) 2022  CAS
 */
#ifndef __WellLineSensor_H_
#define __WellLineSensor_H_

/* Includes ------------------------------------------------------------------*/

/* Public define --------------------------------------------------------—----*/
#define DLCmd_Pack_TOKEN 07
#define DLCmd_Pack_EOT 04

#define Sensor_Pack_TOKEN 06
#define Sensor_Pack_EOT 04

/* Typedef ----------------------------------------------------------————————-*/
typedef struct
{
    unsigned short Ax; //   /10000=真实值
    unsigned short Ay; //   /10000=真实值
    unsigned short Az; //   /10000=真实值

    unsigned short Mx; //   /10000=真实值
    unsigned short My; //   /10000=真实值
    unsigned short Mz; //   /10000=真实值

    unsigned short Gz;    //  gyro z
    unsigned short Gtemp; // gyro sensor temp

    unsigned short Roll;    //   /10=真实值
    unsigned short MagRoll; //   /10=真实值

    unsigned short Inclination; //   /10=真实值
    unsigned short TotMag;      //   /1000=真实值
    unsigned short Azimuth;     //   /10=真实值
    unsigned short TotGrav;     //   /1000=真实值

    unsigned short Temp; // 100=真实值
    unsigned short Volt; // downhole Voltage  /10=真实值
} MWDSensor_RawData_TypeDef;

typedef enum
{
    Sensor_PackType_Identity = 0x00,
    Sensor_PackType_Vector_Data = 0x01,
    Sensor_PackType_Angle_Data = 0x02,
    Sensor_PackType_TopEnd_Data = 0x03,
    Sensor_PackType_Rotational_Data = 0x51,
    Sensor_PackType_Azimuthal_Gamma_Data = 0x71,

    //-----
    Sensor_PackType_RawADC = 0xC1,     // rawadc
    Sensor_PackType_RTC = 0xC2,        // RTC
    Sensor_PackType_File_Infor = 0xC4, // file information
    Sensor_PackType_File_Data = 0xC5,  // file content data

    Sensor_PackType_CMD_Undefine
} Sensor_PackType_TypeDef;

typedef enum
{
    //-----
    DLCmd_PackType_Identity = 0xC0,
    DLCmd_PackType_RealtimeData = 0xC1,   // start or stop to report realtime data
    DLCmd_PackType_SetRTC = 0xC2,         // RTC
    DLCmd_PackType_GetRTC = 0xC3,         // RTC
    DLCmd_PackType_Get_File_Infor = 0xC4, // file information
    DLCmd_PackType_Get_File_Data = 0xC5,  // file content data
    DLCmd_PackType_Del_File = 0xC6,       // delect file
    DLCmd_PackType_USB_Storage = 0xC7,     // start or stop usb storge
	DLCmd_PackType_Reset = 0xD0,    // reset 
    DLCmd_PackType_CMD_Undefine
} DLCmd_PackType_TypeDef;

// <06><00><41><50><53><3A><53><2F><4E><20><Serial Number><CR><LF><56><05><72><3A><20><FirmwareVersion><CR><LF><CRC16><06><04>
// <06> is the token.
// <00> identifies directional sensor identity packet.
// <41><50><53><3A><53><2F><4E><20> prints the message APS: S/N.
// <Serial Number> is the system serial number.
// <CR><LF> is carriage return and line feed.
// <56><05><72><3A><20> prints the message VER:.
// <Firmware version> is the directional sensor firmware version.
// <CR><LF> is carriage return and line feed.
// <CRC> is the 16-bit cyclic redundancy check.
// <06> is the token.
// <04> is EOT.
typedef struct
{
    char SN[18];  // WL610-01-ABCDEFGH
    char Ver[16];
} Identity_Typedef;

typedef struct
{
    unsigned char year;   // 年
    unsigned char month;  // 月
    unsigned char dat;    // 日
    unsigned char hour;   // 时
    unsigned char minute; // 分
    unsigned char second; // 秒
} DatetimeStruct_TypeDef;

// <06> <01> <Status> <Mx> <Ax> <My><Ay> <Mz> <Az> <Temp > <Voltage> <CRC> <06> <04>
typedef struct
{
    // const char start[2] = {APS_TOKEN, 0x01};
    unsigned char Status; // Has only 3 bits defined. The most significant bit indicates the status of the mule shoe input and the lower 2 bits indicate the data correction level of this data (2- Vectors  3- Angles).
    unsigned short Mx;    // Gauss multiplied by 10000
    unsigned short Ax;    // Gees multiplied by 10000
    unsigned short My;    // Gauss multiplied by 10000
    unsigned short Ay;    // Gees multiplied by 10000
    unsigned short Mz;    // Gauss multiplied by 10000
    unsigned short Az;    // Gees multiplied by 10000
    short Temp;           // temperature in degrees Celsius times 100.
    unsigned short Volt;  //  voltage times 10.
    // UWORD CRC16;  // LSB first followed by MSB
    // const char end[2] = {APS_TOKEN, APS_EOT};
} VectorData_Typedef;

typedef struct
{
    // const char start[2] = {APS_TOKEN, 0x02};
    unsigned char HeadStatus; // Has only 3 bits defined. The most significant bit indicates the status of the mule shoe input and the lower 2 bits indicate the data correction level of this data (2- Vectors  3- Angles).
    unsigned short Roll;      // Gravitational Toolface in units of 1/10 degree.
    unsigned short MRoll;     // Magnetic Toolface in units of 1/10 degree.
    unsigned short Inc;       // Inclination in units of 1/10 degree.
    unsigned short TMag;      // Total Magnetic Field in units of 1/10000 gauss.
    unsigned short Azimuth;   // Azimuth in units of 1/10 degree.
    unsigned short TGrav;     // Total Gravitational Field in units of 1/10000 gees.
    unsigned short Temp;      // This is the temperature of the sensor. It is encoded as a two byte signed integer with the value of the temperature in degrees Celsius times 100.
    unsigned short Volt;      // This is the voltage going into the sensor from the wire line connection. It is encoded as a two byte signed integer with a value of the input voltage times 10.
    // UWORD CRC16;
    // const char end[2] = {APS_TOKEN, APS_EOT};
} AngleData_Typedef;

//<06><03><TEUSTATUS><AVE><REFROLL><MAGDEC><TOPVOLT><CRUUENT><ROLL><MAGROLL><INC><TOTMAG><AZI><TOTGRAV><CRC16><06><04>
typedef struct
{
    // const char start[2] = {APS_TOKEN, 0x03};
    unsigned char TeuStatus; // Has only 3 bits defined. The most significant bit indicates the status of the mule shoe input and the lower 2 bits indicate the data correction level of this data (2- Vectors  3- Angles).
    unsigned short AVE;      //
    unsigned short REFROLL;  //
    unsigned short MAGDEC;   //
    unsigned short TOPVOLT;  //
    unsigned short CRUUENT;  //
    unsigned short ROLL;     //
    unsigned short MAGROLL;  //
    unsigned short INC;      //
    unsigned short TOTMAG;   //
    unsigned short AZI;      //
    unsigned short TOTGRAV;  //
    // UWORD CRC16;
    // const char end[2] = {APS_TOKEN, APS_EOT};
} TopEndData_Typedef;

typedef struct
{
    // Rotation Sensor Packet
    // UBYTE TOKEN;			// 0x06
    // UBYTE ID;            // 0x51
    signed short RotationRate;    // Rotation rate in RPM times 10 (signed value)
    unsigned short MagneticRoll;  // Magnetic roll in degrees times 182
    unsigned short MagneticField; // Total transverse magnetic field in Gauss times 32768
    // UWORD CRC;           // CRC-16
    // UBYTE TOKEN;			// 0x06
    // UBYTE END;			// 0x04
} RotationData_Typedef;

typedef struct
{
    // const char start[2] = {TOKEN, 0x71};
    unsigned short GammaUp;    // gamma up counts
    unsigned short GammaDown;  // gamma down counts
    unsigned short GammaLeft;  // gamma left counts
    unsigned short GammaRight; // gamma right counts
    // UWORD CRC16;
    // const char end[2] = {TOKEN, EOT};
} FocusGamma4Data_Typedef;

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
extern unsigned char LastDLCmd;             // 上一次接收到的命令
extern unsigned char LastDLCmd_Payload[32]; // 上一次接收到的附加内容
extern char Str_ID[49];

/* Public functions ---------------------------------------------------------*/
extern unsigned char Receive_Byte(unsigned char _rcvbyte);
extern void Init_ID(void);
extern unsigned short Gen_Tx_Pack_Id(unsigned char *pRetBytes);
extern unsigned short Gen_Tx_Pack_RawADC(unsigned short *_pRawADC, unsigned char *pRetBytes);
extern unsigned short Gen_Tx_Pack_RTC(unsigned char *_rtc, unsigned char *pRetBytes);
extern unsigned short Gen_Tx_Pack_FileInfor(unsigned int filesize, unsigned char *pRetBytes);
extern unsigned short Gen_Tx_Pack_FileData(unsigned char *pfiledata, unsigned short size, unsigned char *pRetBytes);

#endif // __WellLineSensor_H_

/*********** (C) COPYRIGHT 2022 skyhigh *****END OF FILE****/

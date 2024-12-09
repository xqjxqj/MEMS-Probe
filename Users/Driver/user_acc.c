/**
 * @file Untitled-1
 * @brief  一共有8颗 MEMS加速度芯片，4颗ADXL355在传感器模块中，4颗ADXL357在板上,合计有24个通道
 * 4颗ADXL355配置一样，从数据通道nCS、MISO1-4读回，
 * 2颗ADXL357配置一样，从数据通道nCS_2、MISO5-6读回
 * 另2颗ADXL357配置一样，从数据通道nCS_3、MISO7-8读回
 * SCK和MOSI所有芯片共用
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2021-06-08
 * @copyright Copyright (c) 2021  CAS 
 * 
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
#define Dly_100ns 16
#define OneGroupSensorRawBytesLength 11
#define OneGroupChannelSize 8

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t Accelerator_SensorRawBytes[OneGroupChannelSize * OneGroupSensorRawBytesLength]; // 读取回来的原始数值字节
ADXL355_TypeDef Accelerator_SensorRawValue[OneGroupChannelSize];                        // 读取回来的原始数值浮点值

/* Public variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static void delay(int del)
{
    for (; del > 0; del--)
        __NOP();
}

/**
 * @brief  读回所有通道数据，读回的8个字节：byte7=CH8,byte6=CH7...byte0=CH1
 * @param  wr           写入的dummy字节
 * @param  pRdVal       返回8个字节
 */
static void SPI_Write_Read_Byte(uint8_t wr, uint8_t *pRdVal)
{
    uint8_t i, m;
    uint8_t MASK = 0x80;

    for (i = 0; i < 8; i++)
        pRdVal[i] = 0x00;

    for (m = 0; m < 8; m++)
    {
        if (wr & MASK)
            MCU_SPI_MOSI_H;
        else
            MCU_SPI_MOSI_L;

        delay(Dly_100ns);
        MCU_SPI_SCK_H;
        delay(Dly_100ns);

        for (i = 0; i < 8; i++)
            pRdVal[i] <<= 1;

        if (bit_MCU_SPI_MISO_1)
            pRdVal[0] |= 0x01;
        if (bit_MCU_SPI_MISO_2)
            pRdVal[1] |= 0x01;
        if (bit_MCU_SPI_MISO_3)
            pRdVal[2] |= 0x01;
        if (bit_MCU_SPI_MISO_4)
            pRdVal[3] |= 0x01;
        if (bit_MCU_SPI_MISO_5)
            pRdVal[4] |= 0x01;
        if (bit_MCU_SPI_MISO_6)
            pRdVal[5] |= 0x01;
        if (bit_MCU_SPI_MISO_7)
            pRdVal[6] |= 0x01;
        if (bit_MCU_SPI_MISO_8)
            pRdVal[7] |= 0x01;

        MCU_SPI_SCK_L;
        MASK >>= 1;
    }
}

/**
 * @brief  写入一组字节
 * @param  pBytes          写入的数组
 * @param  size            写入字节数
 */
static void SPI_Write_Bytes(uint8_t *pBytes, uint8_t size)
{
    uint8_t i, m;
    uint8_t MASK;

    for (i = 0; i < size; i++)
    {
        MASK = 0x80;
        for (m = 0; m < 8; m++)
        {
            if (pBytes[i] & MASK)
                MCU_SPI_MOSI_H;
            else
                MCU_SPI_MOSI_L;

            delay(Dly_100ns);
            MCU_SPI_SCK_H;
            delay(Dly_100ns);
            MCU_SPI_SCK_L;
            MASK >>= 1;
        }
    }
}

//---------------- 多字节读写---------------
/**
 * @brief  多个字节写入
 * @param  chs              写入的通道
 * @param  ui8address       起始寄存器地址
 * @param  pui8Data         写入的数组
 * @param  datalen          写入的长度
 */
static void SPI_Write_Multi_CHs(SPI_CH_Type chs, uint8_t ui8address, uint8_t *pui8Data, uint8_t datalen)
{
    uint8_t ui8writeAddress = ((ui8address << 1) | ADXL355_WRITE);

    if (chs == SPI_CH_1_4)
        MCU_SPI_nCS_L;
    else if (chs == SPI_CH_5_6)
        MCU_SPI_nCS_2_L;
    else if (chs == SPI_CH_7_8)
        MCU_SPI_nCS_3_L;

    SPI_Write_Bytes(&ui8writeAddress, 1);
    SPI_Write_Bytes(pui8Data, datalen);

    if (chs == SPI_CH_1_4)
        MCU_SPI_nCS_H;
    else if (chs == SPI_CH_5_6)
        MCU_SPI_nCS_2_H;
    else if (chs == SPI_CH_7_8)
        MCU_SPI_nCS_3_H;
}

/**
 * @brief   8通道同时写入数据
 * @param  ui8address       起始寄存器地址
 * @param  pui8Data         写入的数组
 * @param  datalen          写入的长度
 */
static void SPI_Write_Multi(uint8_t ui8address, uint8_t *pui8Data, uint8_t datalen)
{
    uint8_t ui8writeAddress = ((ui8address << 1) | ADXL355_WRITE);

    MCU_SPI_nCS_L;
    MCU_SPI_nCS_2_L;
    MCU_SPI_nCS_3_L;

    SPI_Write_Bytes(&ui8writeAddress, 1);
    SPI_Write_Bytes(pui8Data, datalen);

    MCU_SPI_nCS_H;
    MCU_SPI_nCS_2_H;
    MCU_SPI_nCS_3_H;
}

/**
 * @brief  8通道同时读回
 * @param  ui8address       起始寄存器地址
 * @param  pui8Data         读回数据 pui8Data[0][]=CH1,pui8Data[1][]=CH2 ... pui8Data[7][]=CH8
 * @param  datalen          每通道读回数据长度
 */
static void SPI_Read_Multi(uint8_t ui8address, uint8_t *pui8Data, uint8_t datalen)
{
    uint8_t i, m;
    uint8_t ChVal[8];
    uint8_t ui8writeAddress = ((ui8address << 1) | ADXL355_READ);

    MCU_SPI_nCS_L;
    MCU_SPI_nCS_2_L;
    MCU_SPI_nCS_3_L;

    SPI_Write_Bytes(&ui8writeAddress, 1);

    for (i = 0; i < datalen; i++)
    {
        SPI_Write_Read_Byte(0xff, ChVal); // 8个通道同时读回
        for (m = 0; m < OneGroupChannelSize; m++)
            *(pui8Data + m * datalen + i) = ChVal[m];
    }

    MCU_SPI_nCS_H;
    MCU_SPI_nCS_2_H;
    MCU_SPI_nCS_3_H;
}

/**
 * @brief  初始化加速度传感器
 * @return uint8_t 
 */

uint8_t Accelerator_Init(void)
{
    uint8_t ReadBytes[16];
    uint8_t ui8temp;
    uint8_t ret = 0;

    SPI_Read_Multi(DEVID_AD, ReadBytes, 1); // Read DEVID_AD register
    SPI_Read_Multi(PARTID, ReadBytes, 1);   // Read PARTID register
    SPI_Read_Multi(REVID, ReadBytes, 1);    // Read REVID register

    for (ui8temp = 0; ui8temp < OneGroupChannelSize; ui8temp++)
    {
        if (ReadBytes[ui8temp] != 0)
            ret++;
    }

    // FILTER
    // bit       7    RES
    //           6    5    4
    //000=NO HPF  001=247*10(-3)*ODR  010=62.084*10(-3)*ODR  011=15.545*10(-3)*ODR  100=3.862*10(-3)*ODR  101=0.954*10(-3)*ODR  110=0.238*10(-3)*ODR

    //                           3    2    1    0  ODR&LPF corner
    // 0000=4000/1000 0001=2000/500 0010=1000/250 0011=500/125 0100=250/62.5 0101=125/31.25 0110=62.5/15.625 0111=31.25/7.813 1000=15.625/3.906 1001=7.813/1.953 1010=3.906/0.977

    //ui8temp = 0x07;						   // 0x07 =ODR=31.25Hz,LPF cornor7.8Hz
    //ui8temp = 0x02;						   // 0x02 =ODR=1000Hz,LPF cornor250Hz
    //ui8temp = 0x03;                       // 0x03 =ODR=500Hz,LPF cornor125Hz
    ui8temp = 0x04;						   // 0x04 =ODR=250Hz,LPF cornor62.5Hz
    SPI_Write_Multi(FILTER, &ui8temp, 1); /* Write FILTER register, before modifying it */

    //ui8temp = 0x10;						   //  DATA_RDY interrupt enable on INT2
    //SPI_Write_Multi(INT_MAP, &ui8temp, 1); /* */

    // SPI_Read_Multi(RANGE, ReadBytes, 1); /* Read the ID register */

    //ui8temp = 0xc2;                      //   bit6=1: INT1 INT2 active high, bit1:0=10 range +-20g
    //SPI_Write_Multi(RANGE, &ui8temp, 1); /* */

    // RANGE
    // bit 7   1=IIC high speed 0=IIC fast mode
    // bit 6   1=INT1/INT2 active high  0=INT1/INT2 active low
    // bit 5-2 RES
    // bit 1:0 00 = ADXL355 +-2g    ADXL357 +-10g
    //         01 = ADXL355 +-4g    ADXL357 +-20g
    //         10 = ADXL355 +-8g    ADXL357 +-40g

    ui8temp = 0x41; // ADXL355 +-4g
    SPI_Write_Multi_CHs(SPI_CH_1_4, RANGE, &ui8temp, 1);
    ui8temp = 0x40; // ADXL357 +-10g
    SPI_Write_Multi_CHs(SPI_CH_5_6, RANGE, &ui8temp, 1);
    ui8temp = 0x42; // ADXL357 +-40g
    SPI_Write_Multi_CHs(SPI_CH_7_8, RANGE, &ui8temp, 1);

    return ret;
}

/**
 * @brief 传感器开始采集
 */
void ADXL355_Start_Sensor(void)
{
	uint8_t ui8temp[8];

	SPI_Read_Multi(POWER_CTL, ui8temp, 1); /* Read POWER_CTL register, before modifying it */

	//ui8temp[4] = ui8temp[4] & 0xFE; /* Set measurement bit in POWER_CTL register */
// POWER_CTL  BIT7-3  RES
//  BIT 2   DRDY_OFF  1=DRDY output to 0 
//  BIT 1   TEMP_OFF  1=TEMP_OFF
//  BIT_0   STANDBY   1=STANDBY 
    ui8temp[0] = 0x00;
    SPI_Write_Multi(POWER_CTL, ui8temp, 1); /* Write the new value to POWER_CTL register */
}


static int32_t Acceleration_Bytes_Conversion(uint8_t *pSensorBytes)
{
    int32_t i32Conversion = 0;

    i32Conversion = pSensorBytes[0];
    i32Conversion <<= 8;
    i32Conversion |= pSensorBytes[1];
    i32Conversion <<= 8;
    i32Conversion |= pSensorBytes[2];

    i32Conversion >>= 4;

    if (pSensorBytes[0] & 0x80)
        i32Conversion |= 0xfff00000;

    return i32Conversion;
}

static float Temperature_Bytes_Conversion(uint8_t *pSensorBytes)
{
    int16_t i16Conversion = 0;
    float fVal;

    i16Conversion = pSensorBytes[0] & 0x0F;
    i16Conversion <<= 8;
    i16Conversion |= pSensorBytes[1];

    fVal = i16Conversion;

    fVal = ADXL355_TEMP_Normal + ((fVal - ADXL355_TEMP_BIAS) / ADXL355_TEMP_SLOPE);

    return fVal;
}

// void Accelerator_ReadRawDataBytes(void)
// {
//     SPI_Read_Multi(TEMP2, Accelerator_SensorRawBytes, OneGroupSensorRawBytesLength);
// }

void ADXL355_Data_Scan(void)
{
    int i;
    uint8_t offset;

    SPI_Read_Multi(TEMP2, Accelerator_SensorRawBytes, OneGroupSensorRawBytesLength);

    offset = 0;
    for (i = 0; i < OneGroupChannelSize; i++)
    {
        Accelerator_SensorRawValue[i].fTemperature = Temperature_Bytes_Conversion(&(Accelerator_SensorRawBytes[offset]));
        offset += 2;
        Accelerator_SensorRawValue[i].fAcc_X = Acceleration_Bytes_Conversion(&(Accelerator_SensorRawBytes[offset]));
        offset += 3;
        Accelerator_SensorRawValue[i].fAcc_Y = Acceleration_Bytes_Conversion(&(Accelerator_SensorRawBytes[offset]));
        offset += 3;
        Accelerator_SensorRawValue[i].fAcc_Z = Acceleration_Bytes_Conversion(&(Accelerator_SensorRawBytes[offset]));
        offset += 3;
    }
}

/* Public functions ---------------------------------------------------------*/

/*********** (C) COPYRIGHT 2021  skyhigh  *****END OF FILE****/

/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-04-27
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
#include "iir_filter.h"
#include "iir_coefs.h"

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

// 以下针对从ADC读出的原始数据
// ----------  rawdata ----------
float RawDataCH0_y[RawData_NSEC][3];
float RawDataCH0_x[RawData_NSEC + 1][3];

float RawDataCH1_y[RawData_NSEC][3];
float RawDataCH1_x[RawData_NSEC + 1][3];

float RawDataCH2_y[RawData_NSEC][3];
float RawDataCH2_x[RawData_NSEC + 1][3];

float RawDataCH3_y[RawData_NSEC][3];
float RawDataCH3_x[RawData_NSEC + 1][3];

float RawDataCH4_y[RawData_NSEC][3];
float RawDataCH4_x[RawData_NSEC + 1][3];

float RawDataCH5_y[RawData_NSEC][3];
float RawDataCH5_x[RawData_NSEC + 1][3];

float RawDataCH6_y[RawData_NSEC][3];
float RawDataCH6_x[RawData_NSEC + 1][3];

float RawDataCH7_y[RawData_NSEC][3];
float RawDataCH7_x[RawData_NSEC + 1][3];

/* Public variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

// ------ RawData LPF ------
// 每个通道执行低通滤波
short iir_filter_RawDataCHx(float pRawData_x[RawData_NSEC + 1][3], float pRawData_y[RawData_NSEC][3], float in)
{
    unsigned char i;

    pRawData_x[0][0] = in;
    for (i = 0; i < RawData_NSEC; i++)
    {
        pRawData_y[i][0] = pRawData_x[i][0] * RawData_IIR_B[i][0] + pRawData_x[i][1] * RawData_IIR_B[i][1] + pRawData_x[i][2] * RawData_IIR_B[i][2] - pRawData_y[i][1] * RawData_IIR_A[i][1] - pRawData_y[i][2] * RawData_IIR_A[i][2];
        pRawData_y[i][2] = pRawData_y[i][1];

        pRawData_y[i][1] = pRawData_y[i][0];
        pRawData_x[i][2] = pRawData_x[i][1];
        pRawData_x[i][1] = pRawData_x[i][0];
        pRawData_x[i + 1][0] = pRawData_y[i][0];
    }

    return ((short)pRawData_x[RawData_NSEC][0]);
}


/**
 * @brief 把24bit数据抽取为16bit数据，连续存储
 *
 * @param pIn24BitDataBuf  输入 24bit 数据连续存储 首地址
 * @param pOut16BitDataBuf 输出 16bit 数据连续存储 首地址
 * @param ChSize           通道数
 * @param RecordSize       每通道数据量
 */
void RawData_24b_16b(uint8_t *pIn24BitDataBuf, int16_t *pOut16BitDataBuf, uint8_t ChSize, uint16_t RecordSize)
{
    uint16_t i;
    uint8_t ch;
    uint8_t *pU8;
    int16_t *pI16;

    pI16 = pOut16BitDataBuf;
    for (i = 0; i < RecordSize; i++)
    {
        for (ch = 0; ch < ChSize; ch++)
        {
            pU8 = (uint8_t *)pI16;
            *pU8++ = *pIn24BitDataBuf++;
            *pU8++ = *pIn24BitDataBuf++;
            pIn24BitDataBuf++;

            pI16++;
        }
    }
}

/**
 * @brief 使用CPU内部的FMAC计算
 *
 * @param pRawDataByteBuf
 * @param pOut
 */
void Raw_24BitData_LPF_FMAC(uint8_t *pRawDataByteBuf, int16_t *pOut)
{
    float RawData_float;
    unsigned char *pU8;
    // 把24bit ADC 数据存为连续的16bit 数据，抽取方法是保留高16bit
    // RawData_24b_16b(pRawDataByteBuf, pOut, ExtADC_Chs, ExtADC_RecordLength_Each_Buffer);
}

/**
 * @brief  对24位数据进行LPF ,一次处理8个通道
 *
 * @param pRawDataByteBuf  24位原始数据的起始地址
 * @param pOut             16位滤波数据的起始地址
 */
void AllChs_OneRecord_LPF(uint8_t *pRawDataByteBuf, int16_t *pOut)
{
    float RawData_float;
    unsigned char *pU8IN;

#if 0
    int RawData_Uint32;
    // 以下代码用空间换取执行速度

    //  CH1
    // 24bit数据拼接
    if ((pRawDataByteBuf[0] & 0x80) == 0x80)
        RawData_Uint32 = 0xff000000 | (pRawDataByteBuf[0] << 16) | (pRawDataByteBuf[1] << 8) | (pRawDataByteBuf[2]);
    else
        RawData_Uint32 = (pRawDataByteBuf[0] << 16) | (pRawDataByteBuf[1] << 8) | (pRawDataByteBuf[2]);
    RawData_float = RawData_Uint32;
    pOut[0] = (short)iir_filter_RawDataCHx(RawDataCH0_x, RawDataCH0_y, RawData_float);
    //  CH2
    // 24bit数据拼接
    if ((pRawDataByteBuf[3] & 0x80) == 0x80)
        RawData_Uint32 = 0xff000000 | (pRawDataByteBuf[3] << 16) | (pRawDataByteBuf[4] << 8) | (pRawDataByteBuf[5]);
    else
        RawData_Uint32 = (pRawDataByteBuf[3] << 16) | (pRawDataByteBuf[4] << 8) | (pRawDataByteBuf[5]);
    RawData_float = RawData_Uint32;
    pOut[1] = (short)iir_filter_RawDataCHx(RawDataCH1_x, RawDataCH1_y, RawData_float);
    //  CH3
    // 24bit数据拼接
    if ((pRawDataByteBuf[6] & 0x80) == 0x80)
        RawData_Uint32 = 0xff000000 | (pRawDataByteBuf[6] << 16) | (pRawDataByteBuf[7] << 8) | (pRawDataByteBuf[8]);
    else
        RawData_Uint32 = (pRawDataByteBuf[6] << 16) | (pRawDataByteBuf[7] << 8) | (pRawDataByteBuf[8]);
    RawData_float = RawData_Uint32;
    pOut[2] = (short)iir_filter_RawDataCHx(RawDataCH2_x, RawDataCH2_y, RawData_float);
    //  CH4
    // 24bit数据拼接
    if ((pRawDataByteBuf[9] & 0x80) == 0x80)
        RawData_Uint32 = 0xff000000 | (pRawDataByteBuf[9] << 16) | (pRawDataByteBuf[10] << 8) | (pRawDataByteBuf[11]);
    else
        RawData_Uint32 = (pRawDataByteBuf[9] << 16) | (pRawDataByteBuf[10] << 8) | (pRawDataByteBuf[11]);
    RawData_float = RawData_Uint32;
    pOut[3] = (short)iir_filter_RawDataCHx(RawDataCH3_x, RawDataCH3_y, RawData_float);
    //  CH5
    // 24bit数据拼接
    if ((pRawDataByteBuf[12] & 0x80) == 0x80)
        RawData_Uint32 = 0xff000000 | (pRawDataByteBuf[12] << 16) | (pRawDataByteBuf[13] << 8) | (pRawDataByteBuf[14]);
    else
        RawData_Uint32 = (pRawDataByteBuf[12] << 16) | (pRawDataByteBuf[13] << 8) | (pRawDataByteBuf[14]);
    RawData_float = RawData_Uint32;
    pOut[4] = (short)iir_filter_RawDataCHx(RawDataCH4_x, RawDataCH4_y, RawData_float);
#else
    unsigned short RawData_U16;
    unsigned char *pU8;

    pU8IN = pRawDataByteBuf;

    //  CH1
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++; // RawData_U16小端模式，但原始数据依次是Bit23..Bit0
    *pU8 = *pU8IN++;
    // RawData_U16 = *pU8IN++;
    // RawData_U16 <<= 8;
    // RawData_U16 |= *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[0] = (short)iir_filter_RawDataCHx(RawDataCH0_x, RawDataCH0_y, RawData_float);

    //  CH2
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[1] = (short)iir_filter_RawDataCHx(RawDataCH1_x, RawDataCH1_y, RawData_float);

    //  CH3
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[2] = (short)iir_filter_RawDataCHx(RawDataCH2_x, RawDataCH2_y, RawData_float);

    //  CH4
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[3] = (short)iir_filter_RawDataCHx(RawDataCH3_x, RawDataCH3_y, RawData_float);

    //  CH5
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[4] = (short)iir_filter_RawDataCHx(RawDataCH4_x, RawDataCH4_y, RawData_float);

    //  CH6
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[5] = (short)iir_filter_RawDataCHx(RawDataCH5_x, RawDataCH5_y, RawData_float);

    //  CH7
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[6] = (short)iir_filter_RawDataCHx(RawDataCH6_x, RawDataCH6_y, RawData_float);

    //  CH8
    // 24bit数据截短成16位
    pU8 = (unsigned char *)(&RawData_U16);
    *(pU8 + 1) = *pU8IN++;
    *pU8 = *pU8IN++;
    pU8IN++;
    RawData_float = (short)(RawData_U16);
    pOut[7] = (short)iir_filter_RawDataCHx(RawDataCH7_x, RawDataCH7_y, RawData_float);
#endif
}



/* Public functions ---------------------------------------------------------*/
//复位滤波器(不含原始数据滤波的)
void iir_reset(void)
{
	// uint16_t i, j;

	// for (i = 0; i < IIR_NSEC + 1; i++)
	// {
	// 	for (j = 0; j < 3; j++)
	// 	{
	// 		AccX_x[i][j] = 0;
	// 		AccY_x[i][j] = 0;
	// 	}
	// }

	// for (i = 0; i < IIR_NSEC; i++)
	// {
	// 	for (j = 0; j < 3; j++)
	// 	{
	// 		AccX_y[i][j] = 0;
	// 		AccY_y[i][j] = 0;
	// 	}
	// }

	// for (i = 0; i < LPF_IIR_NSEC + 1; i++)
	// {
	// 	for (j = 0; j < 3; j++)
	// 	{
	// 		AngleTheta_x[i][j] = 0;
	// 		AnglePsi_x[i][j] = 0;
	// 	}
	// }

	// for (i = 0; i < LPF_IIR_NSEC; i++)
	// {
	// 	for (j = 0; j < 3; j++)
	// 	{
	// 		AngleTheta_y[i][j] = 0;
	// 		AnglePsi_y[i][j] = 0;
	// 	}
	// }
}


/**
 * @brief 对原始数据进行滤波
 * 
 * @param pIn24b 
 * @param pOut16b 
 */
void RawData_LPF(uint8_t *pIn24b, int16_t *pOut16b)
{
    uint16_t record_offset;

    for (record_offset = 0; record_offset < ExtADC_RecordLength_Each_Buffer; record_offset++)
    {
        AllChs_OneRecord_LPF(pIn24b, pOut16b);
        pIn24b += 24;  // ExtADC_Chs*ExtADC_Bytes;
        pOut16b += 16; // ExtADC_Chs*2;
    }
}

/*********** (C) COPYRIGHT 2022  skyhigh  *****END OF FILE****/

#include "main.h"

// 70MHz
//#define Tw0 1     // 1.9us
//#define Tw1 65    // 56.9us
//#define Tws 12   // 12.0us

// 80MHz
//#define Tw0 1     // 1.9us   10us> Tw0 > 50ns 
//#define Tw1 70    // 54us          Tw1 ≥ 50us 
//#define Tws 15   // 12.0us         Tws ≥ 10us 

// 160MHz
#define Tw0 2     // 1.9us   10us> Tw0 > 50ns 
#define Tw1 130    // 54us          Tw1 ≥ 50us 
#define Tws 25   // 12.0us         Tws ≥ 10us 


#define Set_GPIO_Input_MagGainX MagGainX_GPIO_Port->MODER &= 0xFFCFFFFF  // // PC10 BIT21:20 = 00
#define Set_GPIO_Output_MagGainX MagGainX_GPIO_Port->MODER |= 0x00100000 // // PC10 BIT21:20 = 01

#define Set_GPIO_Input_MagGainY MagGainY_GPIO_Port->MODER &= 0xFCFFFFFF  // // PC12 BIT25:24 = 00
#define Set_GPIO_Output_MagGainY MagGainY_GPIO_Port->MODER |= 0x01000000 // // PC12 BIT25:24 = 01

#define Set_GPIO_Input_MagGainZ MagGainZ_GPIO_Port->MODER &= 0xFFFFFF3F  // // PB3  BIT7:6 = 00
#define Set_GPIO_Output_MagGainZ MagGainZ_GPIO_Port->MODER |= 0x00000040 // // PB3  BIT7:6 = 01

static void delay_us(int del)
{
    int i;
    for (; del > 0; del--)
    {
        for (i = 0; i < (SystemCoreClock / 50000000); i++)
            __NOP();
    }
}

/**
 * @brief IO口 置1，转换成输入模式，靠电阻上拉至5V
 * 
 * @param ch 
 */
static void ADS8557_Set(unsigned char ch)
{
    if (ch == 0)
    {
        Set_GPIO_Input_MagGainX;
    }
    else if (ch == 1)
    {
        Set_GPIO_Input_MagGainY;
    }
    else if (ch == 2)
    {
        Set_GPIO_Input_MagGainZ;
    }
}

/**
 * @brief IO口 置0，转换成输出模式
 * 
 * @param ch 
 */
static void ADS8557_Reset(unsigned char ch)
{
    if (ch == 0)
    {
        Set_GPIO_Output_MagGainX;
    }
    else if (ch == 1)
    {
        Set_GPIO_Output_MagGainY;
    }
    else if (ch == 2)
    {
        Set_GPIO_Output_MagGainZ;
    }
}

/**
 * @brief 写出一个bit，先 tws宽度的0，然后tw1宽度的1或者tw0宽度的0
 * 
 * @param bit 
 * @param ch 
 */
static void ADS8557_Set_Bit(unsigned char bit, unsigned char ch)
{
    // 0
    ADS8557_Reset(ch);
    delay_us(Tws);

    // 1
    ADS8557_Set(ch);
    if (bit == 0)
        delay_us(Tw0);
    else
        delay_us(Tw1);
}

/**
 * @brief 设定配置参数的数据结构，分6个区域，38bits
 * field        bits            description
 * 0            0 to 11     12-bit start of packet  1000 0000 0001 
 * 1            12 to 13    2-bit function 
                                00: change sense current 
                                01: simulate parameter value 
                                10: program parameter value 
                                11: read parameter value 
 * 2            14 to 15    2-bit parameter 
                                00: second stage gain code 
                                01: first stage gain code 
                                10: output offset code 
                                11: other functions 
 * 3            16 to 17    2-bit dummy 10 
 * 4            18 to 25    8-bit value 
                                Parameter 00 (second stage gain code): 3 LSBs used 
                                Parameter 01 (first stage gain code): 7 LSBs used 
                                Parameter 10 (output offset code): all 8 bits used 
                                Parameter 11 (other functions) 
                                Bit 0 (LSB): master fuse 
                                Bit 1: fuse for production test at Analog Devices 
 * 5             26 to 37   12-bit end of packet 0111 1111 1110 
 * 
 * @param AD855XData 
 * @param function 
 * @param parameter 
 * @param value 
 */
void AD855XDataPacket(AD855XDataStructure *AD855XData, unsigned char function, unsigned char parameter, unsigned char value)
{
    unsigned char i;

    AD855XData->start = START;
    AD855XData->function = function;
    AD855XData->parameter = parameter;
    AD855XData->dummy = DUMMY;
    AD855XData->value = value;
    AD855XData->end = STOP;

    for (i = 0; i < 12; i++)
    {
        if ((AD855XData->start & 0x800) == 0x800)
            AD855XData->SerialBit[i] = 1;
        else
            AD855XData->SerialBit[i] = 0;
        AD855XData->start <<= 1;
    }
    for (i = 0; i < 2; i++)
    {
        if ((AD855XData->function & 0x02) == 0x02)
            AD855XData->SerialBit[i + 12] = 1;
        else
            AD855XData->SerialBit[i + 12] = 0;
        AD855XData->function <<= 1;
    }
    for (i = 0; i < 2; i++)
    {
        if ((AD855XData->parameter & 0x02) == 0x02)
            AD855XData->SerialBit[i + 14] = 1;
        else
            AD855XData->SerialBit[i + 14] = 0;
        AD855XData->parameter <<= 1;
    }
    for (i = 0; i < 2; i++)
    {
        if ((AD855XData->dummy & 0x02) == 0x02)
            AD855XData->SerialBit[i + 16] = 1;
        else
            AD855XData->SerialBit[i + 16] = 0;
        AD855XData->dummy <<= 1;
    }
    for (i = 0; i < 8; i++)
    {
        if ((AD855XData->value & 0x80) == 0x80)
            AD855XData->SerialBit[i + 18] = 1;
        else
            AD855XData->SerialBit[i + 18] = 0;
        AD855XData->value <<= 1;
    }
    for (i = 0; i < 12; i++)
    {
        if ((AD855XData->end & 0x800) == 0x800)
            AD855XData->SerialBit[i + 26] = 1;
        else
            AD855XData->SerialBit[i + 26] = 0;
        AD855XData->end <<= 1;
    }
}

#if 0
void AD855XChangeSenseCurrent(unsigned char value)
{
    xdata AD855XDataStructure AD855XDataTemp;

    unsigned char i;   

    AD855XDataPacket(&AD855XDataTemp, FUNCTION_CHANGE_SENSE_CURRENT, NULL, value);

    for (i = 0; i < 38; i++)
    {
        AD855XDin = 0;
        ADuC841Delay(30);
        AD855XDin = 1;
        if (AD855XDataTemp.SerialBit[i] == 1)
        {
            ADuC841Delay(80);
        }
        else
        {
            ADuC841Delay(3);
        }
    }
    AD855XDin = 0;

    EA = 1; /* enable interrupts */
}

void AD855XProgramParameterValue(unsigned char parameter, unsigned char value)
{
    xdata AD855XDataStructure AD855XDataTemp;

    unsigned char i;
    unsigned char j;
    unsigned char tempValue = 0x01;

    EA = 0; /* clear interrupts */

    for (j = 0; j < 8; j++)
    {
        if ((value & tempValue) == tempValue)
        {
            AD855XDataPacket(&AD855XDataTemp, FUNCTION_PROGRAM_PARAMETER_VALUE, parameter, tempValue);
            
            for (i = 0; i < 38; i++)
            {
                AD855XDin = 0;
                ADuC841Delay(30);
                AD855XDin = 1;
                if (AD855XDataTemp.SerialBit[i] == 1)
                {
                    ADuC841Delay(80);
                }
                else
                {
                    ADuC841Delay(3);
                }
            }
            AD855XDin = 0;
        }
        tempValue = tempValue << 1;

        ADuC841Delay(8000); // delay at least 1ms
    }
    EA = 1; /* enable interrupts */
}

void AD855XBLOWMASTERFUSE(void)
{
    xdata AD855XDataStructure AD855XDataTemp;

    unsigned char i;

    EA = 0; /* clear interrupts */

    AD855XDataPacket(&AD855XDataTemp, FUNCTION_PROGRAM_PARAMETER_VALUE, PARAMETER_OTHER_FUNCTIONS, MASTER_FUSE);

    for (i = 0; i < 38; i++)
    {
        AD855XDin = 0;
        ADuC841Delay(30);
        AD855XDin = 1;
        if (AD855XDataTemp.SerialBit[i] == 1)
        {
            ADuC841Delay(80);
        }
        else
        {
            ADuC841Delay(3);
        }
    }
    AD855XDin = 0;

    ADuC841Delay(8000); // delay at least 1ms

    EA = 1; /* enable interrupts */
}

// only for AD8556
void AD855XBlowParityFuse(void)
{
    xdata AD855XDataStructure AD855XDataTemp;

    unsigned char i;

    EA = 0; /* clear interrupts */

    AD855XDataPacket(&AD855XDataTemp, FUNCTION_PROGRAM_PARAMETER_VALUE, PARAMETER_OTHER_FUNCTIONS, PARITY_FUSE);

    for (i = 0; i < 38; i++)
    {
        AD855XDin = 0;
        ADuC841Delay(30);
        AD855XDin = 1;
        if (AD855XDataTemp.SerialBit[i] == 1)
        {
            ADuC841Delay(80);
        }
        else
        {
            ADuC841Delay(3);
        }
    }
    AD855XDin = 0;

    ADuC841Delay(8000); // delay at least 1ms

    EA = 1; /* enable interrupts */
}

unsigned char AD855XReadParameterValue(unsigned char parameter)
{
    xdata AD855XDataStructure AD855XDataTemp;

    unsigned char i;
    unsigned char tempValue = 0x00;

    EA = 0; /* clear interrupts */

    AD855XDataPacket(&AD855XDataTemp, FUNCTION_READ_PARAMETER_VALUE, parameter, NULL);

    for (i = 0; i < 38; i++)
    {
        AD855XDin = 0;
        ADuC841Delay(30);
        AD855XDin = 1;
        if (AD855XDataTemp.SerialBit[i] == 1)
        {
            ADuC841Delay(80);
        }
        else
        {
            ADuC841Delay(3);
        }
    }

    for (i = 0; i < 8; i++)
    {
        AD855XDin = 0;
        ADuC841Delay(30);
        tempValue = (tempValue << 1) | AD855XDOUT; // read
        AD855XDin = 1;
        ADuC841Delay(30);
    }

    AD855XDin = 0;

    EA = 1; /* enable interrupts */
    return tempValue;
}
#endif

/**
 * @brief   Simulation Mode，
 * 
 * @param parameter 
 * @param value 
 * @param ch 
 */
void AD855XSimulateParameterValue(unsigned char parameter, unsigned char value, unsigned char ch)
{
    AD855XDataStructure AD855XDataTemp;

    unsigned char i;

    AD855XDataPacket(&AD855XDataTemp, FUNCTION_SIMULATE_PARAMETER_VALUE, parameter, value);

    for (i = 0; i < 38; i++)
    {
        ADS8557_Set_Bit(AD855XDataTemp.SerialBit[i], ch);
    }

    ADS8557_Reset(ch);
    delay_us(10);
}

/**
 * @brief Set the Mag Gain 默认Gain=200 ,Gain1=5,Gain2=40,Voffset=1.25V
 *        Vout = (Vp-Vn)*Gfirst*Gsec + Voffset
 * @param FirstStageGainCode  0-127   Gain : 2.8 - 5.2
 * @param SecondStageGainCode 0-7     Gain : 10 - 250
 * Second Stage Gain Code || Second Stage Gain || Minimum Combined Gain || Maximum Combined Gain 
            0                       10                      28.0                52.0 
            1                       16                      44.8                83.2 
            2                       25                      70.0                130.0 
            3                       40                      112.0               208.0 
            4                       63                      176.4               327.6 
            5                       100                     280.0               520.0 
            6                       160                     448.0               832.0 
            7                       250                     700.0               1300.0 

 * @param OffsetCode  Voffset = (OffsetCode+0.5)/256*(VDD-VSS)+VSS = (OffsetCode+0.5)/256 * 5
 * @param ch 
 */
void SetMagGain(unsigned char FirstStageGainCode, unsigned char SecondStageGainCode, unsigned char OffsetCode, unsigned char ch)
{
    AD855XSimulateParameterValue(PARAMETER_FIRST_STAGE_GAIN_CODE, FirstStageGainCode, ch);
    AD855XSimulateParameterValue(PARAMETER_SECOND_STAGE_GAIN_CODE, SecondStageGainCode, ch);
    AD855XSimulateParameterValue(PARAMETER_OUTPUT_OFFSET_CODE, OffsetCode, ch);
}

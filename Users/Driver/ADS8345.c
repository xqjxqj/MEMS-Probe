
#include "main.h"

#define SysCLK 160

// extern SPI_HandleTypeDef hspi1;

#define hSPI hspi1
#define SPI1_TIMEOUT 100000

#if SysCLK==160
	#define SCK_Delay 6    // 160MHz 主频 最低
#else
	#define SCK_Delay 1 // 80MHz 主频 最低
#endif


// 通道选择表
uint8_t ADS8345ChannelRef[]={0x00,0x04,0x01,0x05,0x02,0x06,0x03,0x07};

static void adc_delay(int del)
{
  for (; del > 0; del--)
    __NOP();
}

unsigned short ADS8345_GetData(unsigned char ch)
{
#if 0
  unsigned short ret;
  unsigned char dat[2];

  // start
  dat[0] = (ch << 4) | 0x86;
  HAL_SPI_Transmit(&hSPI, dat, 1, SPI1_TIMEOUT);
  // delay();
  delay(50);
  HAL_SPI_Receive(&hSPI, dat, 2, SPI1_TIMEOUT);

  ret = dat[0];
  ret <<= 8;
  ret |= dat[1];
  return ret;
#else
  unsigned short ret = 0;
  unsigned char MASK = 0x80, i, dat[2];

  SPI1_SCK_L;
  
#if SysCLK==160
	adc_delay(10); // 160MHz 主频 最低
#else
	adc_delay(1); // 80MHz 主频 最低
#endif

  dat[0] = (ch << 4) | 0x86;

  for (i = 0; i < 8; i++)
  {
    if (dat[0] & MASK)
      SPI1_MOSI_H;
    else
      SPI1_MOSI_L;

    SPI1_SCK_H; //__NOP();__NOP();
    adc_delay(SCK_Delay);
    SPI1_SCK_L; //__NOP();
    adc_delay(SCK_Delay);

    dat[0] <<= 1;
  }

#if SysCLK==160
	adc_delay(60); // 160MHz 主频 最低
#else
	adc_delay(30); // 80MHz 主频 最低
#endif 
  
  for (i = 0; i < 16; i++)
  {
    ret <<= 1;

    SPI1_SCK_H; //__NOP();__NOP();
    adc_delay(SCK_Delay);
    SPI1_SCK_L; //__NOP();__NOP();
    adc_delay(SCK_Delay);

    if (bSPI1_MISO)
      ret |= 0x01;
  }

  return ret;
#endif
}

/**
 * @brief 获取3片ADC，共24通道数据，每通道16bit
 *
 * @param pData
 */
void ExtADC_GetResult(unsigned short *pData)
{
  unsigned char i;

#if 1
  ADC1_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ = ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC1_CS_H;

  adc_delay(SCK_Delay);

  ADC2_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ = ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC2_CS_H;

  adc_delay(SCK_Delay);

  ADC3_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ = ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC3_CS_H;
#else
    ADC1_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ = 0;// ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC1_CS_H;

  adc_delay(SCK_Delay);

  ADC2_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ =  ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC2_CS_H;
  
    adc_delay(SCK_Delay);

  ADC3_CS_L;
  for (i = 0; i < 8; i++)
    *pData++ = 0;// ADS8345_GetData(ADS8345ChannelRef[i]);
  ADC3_CS_H;
  
#endif
}

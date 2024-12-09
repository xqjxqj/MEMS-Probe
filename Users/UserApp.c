/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2020-07-14
 * @copyright Copyright (c) 2020  CAS
 ***************************************************************
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2019-11-17 <td>1.0     <td>wangh     <td>内容
 * </table>
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "UserApp.h"
#include "fsmc_nand.h"
#include "FS_Config_NAND_0.h"
#include "AttitudeSolve.h"
#include <math.h>
/* Private define ------------------------------------------------------------*/

// stack size must be multiple of 8 Bytes
#define USER_APP_Init_STK_SZ (5120U)
#define WINDOW_SIZE 5
#define N_MAX 8
#define INCTIME 10
#define SampleRate 50
#define PI 3.14159265358979323846f
uint64_t user_app_init_stk[USER_APP_Init_STK_SZ / 8];
const osThreadAttr_t user_app_init_attr = {
    .stack_mem = &user_app_init_stk[0],
    .stack_size = sizeof(user_app_init_stk)};

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim1, htim2,htim4;
extern OPAMP_HandleTypeDef hopamp4;
extern OPAMP_HandleTypeDef hopamp5;
extern ADC_HandleTypeDef hadc5;
uint16_t vbat[2] __attribute__((section(".ARM.__at_0x20000080")));
//uint16_t vbat[2];
//unsigned short vbat[2];

volatile unsigned char bTim1 = 0;

volatile short RawExtADCData[24];
volatile short RawIntADCData[24];
volatile unsigned short RawGrammaData; // 没赋值

volatile short IncAzi[4];//存放井斜和方位，分别两个short类型

// float 	fExtADCData[24],fIntADCData[24];
DiffAccDataTypeDef fDiffAccData;
AccDataTypeDef fAccData;
MagDataTypeDef fMagData;
float fRotate;
float fTemperature;

AllDataTypeDef SensorData;
AccRefTypeDef AccSensorRef; // V1V8ANA / 2

volatile int RawMagData[3];
volatile int RawMagSet[3], RawMagReset[3];
volatile int MagIndex = 0;

// 写flash缓冲区
volatile unsigned char Flash_Wr_Buf[4096];
volatile unsigned short Flash_Wr_Offset = 0;

/* --------------------------------wkg修改内容------------------------------------*/
//realtime data flag
uint8_t bNew_Realtime_RawData = 0;

//uint16_t Realtime_RawData[24]; // 存一组实时数据:必须要用字节流上传
volatile unsigned char Realtime_RawData[48]; //存一组实时数据
volatile short Realtime_RawData_Temp[24]={0};//用于计算实时数据的均值（用于静态）

//uint16_t Realtime_RawData[ExtADC_Chs + IntADC_VT_Chs];
//uint16_t *pU16ExtADCRaw; // 实时数据，外部ADC数据地址
// Sample rate

// correction coefficient
// gyro: real gyro = measure gyro/(1+GyroCal1)-GyroCal2;
volatile float GyroCal1 = 1.0330f;
volatile float GyroCal2 = 3.094f;
volatile float Static = 10.1598f;

// 姿态角度
volatile float StaticToolface;  // 静态工具面角
volatile float DynamicToolface; // 动态工具面角
volatile float IncGz;           // 井斜
volatile float AvgInclination;
short IncCalTime = SampleRate * INCTIME; // 每隔一段时间算一次井斜
short IncCount = 0;
float StaticGx = 0;
float StaticGy = 0;
uint8_t isCal = 0;
// 存储历史采样值：用于判断磁波峰
volatile short StoreMagXRawData[WINDOW_SIZE] = {0};
volatile short StoreMagYRawData[WINDOW_SIZE] = {0};

// 用于在识别波峰波谷中对磁数据移动平均
volatile int SumOfStoreMagXRawData = 0;
volatile int SumOfStoreMagYRawData = 0;
volatile unsigned int SampleCount = 0;

// 存储波峰下标
volatile unsigned int MagXPeaksIndex[N_MAX] = {0};
volatile unsigned int MagXValleysIndex[N_MAX] = {0};
volatile unsigned int MagYPeaksIndex[N_MAX] = {0};
volatile unsigned int MagYValleysIndex[N_MAX] = {0};
// 取n个周期平均值
volatile uint8_t N_Period;

// 各项转速
volatile float GyroRotation = 0;
volatile float MagYPeaksRotation = 0;
volatile float MagXPeaksRotation = 0;
volatile float MagYValleysRotation = 0;
volatile float MagXValleysRotation = 0;
/* --------------------------------wkg修改内容------------------------------------*/

extern uint32_t PageIndex;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint8_t i;

    if (htim == &htim1)
    {
        bTim1 = 0x01;
    }
		else if (htim == &htim4)
		{
			if(vbat[0]<1980) //BAT1小于26V，切换成BAT2
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
				}
			
		}
}

void Init_AccRef(void)
{
#if Board_SN == 230601
    AccSensorRef.ADC_B_Ref = 2497.0f;
    AccSensorRef.AccLRPRef = 908.5f;   // B: adxl354_1
    AccSensorRef.AccLRNRef = 906.5f;   // B: adxl354_2
    AccSensorRef.AccLR45PRef = 904.5f; // B: adxl354_3
    AccSensorRef.AccLR45NRef = 890.0f; // B: adxl354_4
    AccSensorRef.AccVibPRef = 955.0f;  // B: adxl356C

    AccSensorRef.ADC_A_Ref = 2497.0f;
    AccSensorRef.AccHRPRef = 891.5f; // A: adxl356B_1
    AccSensorRef.AccHRNRef = 896.0f; // A: adxl356B_2

    AccSensorRef.GyroRef = 1348.0f; // A: gyro

    AccSensorRef.TempRef = 982.f;
#elif Board_SN == 230602
    AccSensorRef.ADC_B_Ref = 2493.0f;
    AccSensorRef.AccLRPRef = 900.0f;   // B: adxl354_1
    AccSensorRef.AccLRNRef = 889.0f;   // B: adxl354_2
    AccSensorRef.AccLR45PRef = 904.0f; // B: adxl354_3
    AccSensorRef.AccLR45NRef = 889.0f; // B: adxl354_4
    AccSensorRef.AccVibPRef = 977.0f;  // B: adxl356C

    AccSensorRef.ADC_A_Ref = 2492.0f;
    AccSensorRef.AccHRPRef = 974.5f; // A: adxl356B_1
    AccSensorRef.AccHRNRef = 981.5f; // A: adxl356B_2

    AccSensorRef.GyroRef = 1204.0f; // A: gyro
    AccSensorRef.TempRef = 982.f;
#elif Board_SN == 230603
    AccSensorRef.ADC_B_Ref = 2502.0f;
    AccSensorRef.AccLRPRef = 910.0f;   // B: adxl354_1
    AccSensorRef.AccLRNRef = 906.0f;   // B: adxl354_2
    AccSensorRef.AccLR45PRef = 893.5f; // B: adxl354_3
    AccSensorRef.AccLR45NRef = 892.5f; // B: adxl354_4
    AccSensorRef.AccVibPRef = 974.0f;  // B: adxl356C

    AccSensorRef.ADC_A_Ref = 2492.0f;
    AccSensorRef.AccHRPRef = 974.5f; // A: adxl356B_1
    AccSensorRef.AccHRNRef = 981.5f; // A: adxl356B_2

    AccSensorRef.GyroRef = 1345.0f; // A: gyro
    AccSensorRef.TempRef = 982.f;
#endif
}
void AvgOfRawData(void){
	if(Realtime_RawData_Temp[0]==0){
		//初始化
		for(int rr=0;rr<24;rr++){
		Realtime_RawData_Temp[rr]=RawExtADCData[rr];
	}
  }else{
		for(int rr=0;rr<24;rr++){
		Realtime_RawData_Temp[rr]=Realtime_RawData_Temp[rr]/2+RawExtADCData[rr]/2;
	  }
	}
}
// 更新储存的磁数组,计算磁转速度
void RotationFromRawMagData(void)
{
    SumOfStoreMagXRawData -= StoreMagXRawData[0];
    SumOfStoreMagYRawData -= StoreMagYRawData[0];
    SumOfStoreMagXRawData += RawExtADCData[10] / WINDOW_SIZE;
    SumOfStoreMagYRawData += RawExtADCData[9] / WINDOW_SIZE;
    for (uint16 i = 0; i < WINDOW_SIZE - 1; i++)
    {
        StoreMagXRawData[i] = StoreMagXRawData[i + 1];
        StoreMagYRawData[i] = StoreMagYRawData[i + 1];
    }
    // 滑动平均，将最新值存入数组末
    StoreMagXRawData[WINDOW_SIZE - 1] = SumOfStoreMagXRawData / WINDOW_SIZE;
    StoreMagYRawData[WINDOW_SIZE - 1] = SumOfStoreMagYRawData / WINDOW_SIZE;
    RawExtADCData[10] = SumOfStoreMagXRawData / WINDOW_SIZE;
    RawExtADCData[9] = SumOfStoreMagYRawData / WINDOW_SIZE;
    // MagX
    if ((StoreMagXRawData[WINDOW_SIZE - 2] > StoreMagXRawData[WINDOW_SIZE - 1]) && (StoreMagXRawData[WINDOW_SIZE - 2] > StoreMagXRawData[WINDOW_SIZE - 3]))
    {
        // 是波峰,存入波峰数组

        for (uint16 i = 0; i < N_MAX - 1; i++)
        {
            MagXPeaksIndex[i] = MagXPeaksIndex[i + 1];
        }
        MagXPeaksIndex[N_MAX - 1] = SampleCount;
        if (MagXPeaksIndex[N_MAX - 1 - N_Period] != 0)
        {
            // 计算平均转速
            MagXPeaksRotation = N_Period * 360.0f / (MagXPeaksIndex[N_MAX - 1] - MagXPeaksIndex[N_MAX - 1 - N_Period]) * SampleRate;
        }
    }

    if ((StoreMagXRawData[WINDOW_SIZE - 1] > StoreMagXRawData[WINDOW_SIZE - 2]) && (StoreMagXRawData[WINDOW_SIZE - 3] > StoreMagXRawData[WINDOW_SIZE - 2]))
    {
        // 是波谷
        for (uint16 i = 0; i < N_MAX - 1; i++)
        {
            MagXValleysIndex[i] = MagXValleysIndex[i + 1];
        }
        MagXValleysIndex[N_MAX - 1] = SampleCount;
        if (MagXValleysIndex[N_MAX - 1 - N_Period] != 0)
        {
            // 计算平均转速
            MagXValleysRotation = N_Period * 360.0f / (MagXValleysIndex[N_MAX - 1] - MagXValleysIndex[N_MAX - 1 - N_Period]) * SampleRate;
        }
    }

    // MagY
    if ((StoreMagYRawData[WINDOW_SIZE - 2] > StoreMagYRawData[WINDOW_SIZE - 1]) && (StoreMagYRawData[WINDOW_SIZE - 2] > StoreMagYRawData[WINDOW_SIZE - 3]))
    {
        // 是波峰,存入波峰数组

        for (uint16 i = 0; i < N_MAX - 1; i++)
        {
            MagYPeaksIndex[i] = MagYPeaksIndex[i + 1];
        }
        MagYPeaksIndex[N_MAX - 1] = SampleCount;
        if (MagYPeaksIndex[N_MAX - 1 - N_Period] != 0)
        {
            // 计算平均转速
            MagYPeaksRotation = N_Period * 360.0f / (MagYPeaksIndex[N_MAX - 1] - MagYPeaksIndex[N_MAX - 1 - N_Period]) * SampleRate;
        }
    }

    if ((StoreMagYRawData[WINDOW_SIZE - 1] > StoreMagYRawData[WINDOW_SIZE - 2]) && (StoreMagYRawData[WINDOW_SIZE - 3] > StoreMagYRawData[WINDOW_SIZE - 2]))
    {
        // 是波谷
        for (uint16 i = 0; i < N_MAX - 1; i++)
        {
            MagYValleysIndex[i] = MagYValleysIndex[i + 1];
        }
        MagYValleysIndex[N_MAX - 1] = SampleCount;
        if (MagYValleysIndex[N_MAX - 1 - N_Period] != 0)
        {
            // 计算平均转速
            MagYValleysRotation = N_Period * 360.0f / (MagYValleysIndex[N_MAX - 1] - MagYValleysIndex[N_MAX - 1 - N_Period]) * SampleRate;
        }
    }
}

// 寻找最优周期
uint8_t CalParm_n(float gyrov)
{

    float mingap = 1.0f;
    uint8_t n_min = 0;
    for (uint8_t wkg = 1; wkg < N_MAX; wkg++)
    {
        float Ans = wkg * 360 * SampleRate / gyrov;
        float Ans_Int = roundf(Ans);
        if (fabsf(Ans - Ans_Int) < 0.1f)
        {
            return wkg;
        }
        else
        {
            if (fabs(Ans - Ans_Int) < mingap)
            {
                n_min = wkg;
                mingap = fabs(Ans - Ans_Int);
            }
        }
    }
    return n_min;
}

/**
 * @brief 把外部ADC的原始值转换成未校正的实际值
 *
 */
void ConvertExtADC(void)
{ // 直接将校正值赋给RawExtADCData
    float ftmp;

    // ExtADC ADS8345 Vref=2.5V
    // 0-2*Vref = 0-5V = -32768 ~ 32767

    // rotate转速
    ftmp = RawExtADCData[6];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.GyroRef) / 0.67f; // dps
    fRotate = ftmp;

    // temperature温度
    ftmp = RawExtADCData[7]; // Vout=892.2+3.0*（T-25）/967  //  AccSensorRef.ADC_A_Ref = 2497，AccSensorRef.TempRef=982
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.TempRef) / 3.0f + 25.0f;
    fTemperature = ftmp;

    // HR
    // HR1_X  356B_1_X x加速度1
    ftmp = RawExtADCData[0]; // Vout=V1P8A/2+(0.125g /℃*(T-25) + G) * 80mV/g * (0.01*(T-25))
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRPRef) / 80.0f;
   // fDiffAccData.AccHxP = ftmp;
fDiffAccData.AccHxP = ftmp;//xqj
    // HR1_Y  -356B_1_Z y加速度1
    ftmp = RawExtADCData[1];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRPRef) / 80.0f;
    fDiffAccData.AccHyP = 0 - ftmp;
//fDiffAccData.AccHyP =  ftmp;//xqj
    // HR1_Z  -356B_1_Y z加速度1
    ftmp = RawExtADCData[2];
    // ftmp = (ftmp / 32768.0f * 2500.0f + 2500.0f  - 900.0f) / 80.0f;
    // ftmp = ftmp / 1048.576f + 20.0f;
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRPRef) / 80.0f;
   // fDiffAccData.AccHzP = 0 - ftmp;
 fDiffAccData.AccHzP = 0-ftmp;//xqj
    // HR2_X  -356B_2_Y x加速度2
    ftmp = RawExtADCData[3];
    //fDiffAccData.AccHxN = ftmp;
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRNRef) / 80.0f;
    fDiffAccData.AccHxN = 0 - ftmp;
   //fDiffAccData.AccHxN =  ftmp;
    // HR2_Y  356B_2_Z y加速度2
    ftmp = RawExtADCData[4];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRNRef) / 80.0f;
    fDiffAccData.AccHyN =  ftmp;//???
     //fDiffAccData.AccHyN =  ftmp; //XQJ
    // HR2_Z  356B_2_X z加速度2
    ftmp = RawExtADCData[5];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRNRef) / 80.0f;
    fDiffAccData.AccHzN = ftmp;
//fDiffAccData.AccHzN = 0 - ftmp;//xqj
    // mag  // 传感器100000nT输出5mV，放大800倍
    // MAG_X
    ftmp = RawExtADCData[8];
    ftmp = ftmp * 1.907f;
    fMagData.MagZ = 0 - ftmp;

    // MAG_Y
    ftmp = RawExtADCData[9];
    ftmp = ftmp * 1.907f;
    fMagData.MagY = ftmp;

    // MAG_Z
    ftmp = RawExtADCData[10];
    ftmp = ftmp * 1.907f;
    fMagData.MagX = ftmp;

    // LR Acc
    // LR1_X -354_1_Z  x加速度3
    ftmp = RawExtADCData[11];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRPRef) / 400.0f;
    fDiffAccData.AccLxP = 0 - ftmp;
     //fDiffAccData.AccLxP =  ftmp;//XQJ
    // LR1_Y -354_1_X y加速度3
    ftmp = RawExtADCData[12];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRPRef) / 400.0f;
    fDiffAccData.AccLyP = 0 - ftmp;
//fDiffAccData.AccLyP = ftmp;//xqj
    // LR1_Z -354_1_Y z加速度3
    ftmp = RawExtADCData[13];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRPRef) / 400.0f;
    fDiffAccData.AccLzP = 0 - ftmp;
//fDiffAccData.AccLzP = ftmp;//xqj


    // LR2_X  354_2_Z
    ftmp = RawExtADCData[14];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRNRef) / 400.0f;
    fDiffAccData.AccLxN = ftmp;
     //fDiffAccData.AccLxN = 0 - ftmp;//XQJ
    // LR2_Y 354_2_Y
    ftmp = RawExtADCData[15];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRNRef) / 400.0f;
    fDiffAccData.AccLyN = ftmp;
    //fDiffAccData.AccLyN = 0 - ftmp;//xqj
    // LR2_Z  354_2_X
    ftmp = RawExtADCData[16];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLRNRef) / 400.0f;
    fDiffAccData.AccLzN = ftmp;
    //fDiffAccData.AccLzN = 0 - ftmp;//xqj
    // LR 45
    // LR3_X -354_3_Z
    ftmp = RawExtADCData[17];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45xP = 0 - ftmp;
   // fDiffAccData.AccL45xP =  ftmp;//XQJ
    // LR3_Y*  354_3_Y
    ftmp = RawExtADCData[18];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45yP = ftmp;
//fDiffAccData.AccL45yP = 0 - ftmp;//XQJ
    // LR3_Z*  354_3_X
    ftmp = RawExtADCData[19];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45zP = ftmp;//XQJ
//fDiffAccData.AccL45zP = 0 - ftmp;
    // LR4_X  354_4_Z
    ftmp = RawExtADCData[20];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45xN = ftmp;
//fDiffAccData.AccL45xN = 0-ftmp;//XQJ
    // LR4_Y*  354_4_X
    ftmp = RawExtADCData[21];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45yN = ftmp;
//fDiffAccData.AccL45yN =0- ftmp;//xqj
    // LR4_Z*  354_4_Y
    ftmp = RawExtADCData[22];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45zN = ftmp;
//fDiffAccData.AccL45zN = 0 - ftmp;//xqj
    // current
    ftmp = RawExtADCData[23];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref);
    SensorData.Current = ftmp;
}

/* Public functions ---------------------------------------------------------*/
__NO_RETURN void user_app_init(void *arg)
{
    char *sp, *cp, *next;
    uint32_t i;
 
    unsigned char tmpc[512];

    (void)arg;

    osDelay(1500);

    // 	  Get_ParaInf();
    SenV_ON;
    MagV_ON;
    Shk_ON;

    Init_AccRef();

#if 1
    FlashInit();
    
#endif

    Storage_Init("N0");

    osDelay(500);

    if (bUSBPlug)
    {
        USBD_Initialize(0U); // MSC
        USBD_Connect(0U);

        while (1)
        {
            LED_G_FLASH;
            osDelay(500);
        }
    }

    // -------------------
    Init_RTC(); // 实时日期

    init_cmd_ringbuffer();

    Init_ID();

    User_UART_Start(Huart3_Index);

    Com_Send_Data((uint8_t *)Str_ID, sizeof(Str_ID));

    LED_R_OFF;
    LED_G_OFF;
		vbat[0] = 2100;
		vbat[1] = 2100;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		
		
		HAL_OPAMP_Start(&hopamp4);
	  HAL_OPAMP_Start(&hopamp5);
		HAL_TIM_Base_Start_IT(&htim4);
		HAL_ADC_Start_DMA(&hadc5,(uint32_t*)vbat,2);
		
		
		

    // HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_Base_Start_IT(&htim1); // 开启ExtADC 定期采样
		User_UART_Start(Huart1_Index);
    Init_MWD_APP();
    //Creat_user_app_CMD_task();
		
	
		/*uint16_t SendLength;
		uint8_t SendBuf[128];
		
    
   User_UART_Start(Huart3_Index);
   SendLength = sprintf((char *)SendBuf, "%s", "helll STM32G474!");
   Com_Send_Data(SendBuf, SendLength);
   SendLength = 0;
		*/
    while (1)
    {
        Get_RTC_DDHHMMSS(rtc_val);
        Storage_SetfsDateTime(rtc_val);
			
			  Send_Realtime_RawData(); // XQJ
			
        Decode_DL_CMD();
			 
        if (bUSBStorage == 0x01)
        {
            osDelay(20);

            USBD_Initialize(0U); // MSC
            USBD_Connect(0U);

            bUSBStorage = 0;
        }
        else if (bUSBStorage == 0x02)
        {
            USBD_Disconnect(0U);
            USBD_Uninitialize(0U); // MSC

            bUSBStorage = 0;
        }

#if 1
        // 采集数据

        if (bTim1 == 0x01)
        {
            bTim1 = 0x00;

            // EventStartA(0);

            LED_R_ON;
            // 采集数据：数据赋给RawExtADCData
            ExtADC_GetResult((unsigned short *)RawExtADCData); // 用时 1.5ms
            AvgOfRawData();                                               // 采样点数
            SampleCount++;

            // 数据转换
            ConvertExtADC();
            // 计算井斜角:多次计算取均值
					/*
            if (IncCount <= IncCalTime)
            {
                IncCount++;
                IncGz += fDiffAccData.AccLzP;
            }
            else
            {
                AvgInclination = acos((IncGz / IncCount) / 1.0f) / PI * 180.0f;
                IncCount = 0;
            }

            // 陀螺转速
            GyroRotation = gyroRotation(fRotate, Static, GyroCal1, GyroCal2);
            // 计算最优周期
            N_Period = CalParm_n(GyroRotation);
            */
						
						
            // 提取磁转速
						/*
            if (GyroRotation > 10.0f || GyroRotation < -10.0f)
            {
                // 动态：转速不为0
                if (isCal == 0)
                {
                    StaticToolface = GravityToolface(StaticGx, StaticGy);
                    DynamicToolface = StaticToolface;

                    StaticGx = 0;
                    StaticGy = 0;
                    isCal = 1;
                }

                RotationFromRawMagData();
                DynamicToolface = calculateCurrentAngle(DynamicToolface, GyroRotation / SampleRate);
            }
            else
            {
                // 静态重置
                if (isCal == 1)
                {
                    for (uint16 ii = 0; ii < N_MAX; ii++)
                    {
                        MagXPeaksIndex[ii] = 0;
                        MagXValleysIndex[ii] = 0;
                        MagYPeaksIndex[ii] = 0;
                        MagYValleysIndex[ii] = 0;
                    }
                    for (uint16 ii = 0; ii < WINDOW_SIZE; ii++)
                    {
                        StoreMagXRawData[ii] = 0;
                        StoreMagXRawData[ii] = 0;
                    }
                    SumOfStoreMagXRawData = 0;
                    SumOfStoreMagYRawData = 0;
                    MagXPeaksRotation = 0;
                    MagXValleysRotation = 0;
                    MagYPeaksRotation = 0;
                    MagYValleysRotation = 0;
                    isCal = 0;
                }
                // 计算静态工具面角:求gy\gx的均值
                if (StaticGx == 0)
                    StaticGx = fDiffAccData.AccLxP;
                else
                {
                    StaticGx = StaticGx / 2.0f + fDiffAccData.AccLxP / 2.0f;
                }

                if (StaticGy == 0)
                    StaticGy = fDiffAccData.AccLyP;
                else
                {
                    StaticGx = StaticGy / 2.0f + fDiffAccData.AccLyP / 2.0f;
                }
            }
            */
            // 存校正后的陀螺转速等数据
						/*
            splitFloat(GyroRotation, &RawExtADCData[21], &RawExtADCData[22]);
            splitFloat(MagXPeaksRotation, &RawExtADCData[19], &RawExtADCData[20]);
            splitFloat(MagXValleysRotation, &RawExtADCData[17], &RawExtADCData[18]);
            splitFloat(MagYPeaksRotation, &RawExtADCData[15], &RawExtADCData[16]);
            splitFloat(MagYValleysRotation, &RawExtADCData[13], &RawExtADCData[14]);
            splitFloat(AvgInclination, &RawExtADCData[8], &RawExtADCData[9]);
            splitFloat(DynamicToolface, &RawExtADCData[10], &RawExtADCData[11]);
            RawExtADCData[12] = N_Period;
						*/ //XQJ 
            LED_R_OFF;
						
						splitFloat(mwd2_data_tx.inc, &RawExtADCData[21], &RawExtADCData[22]);
						splitFloat(mwd2_data_tx.azi, &RawExtADCData[21], &RawExtADCData[22]);

            if (MagIndex < 2)
            {
                RawMagSet[0] += RawExtADCData[8];
                RawMagSet[1] += RawExtADCData[9];
                RawMagSet[2] += RawExtADCData[10];
            }
            else if (MagIndex >= 2 && MagIndex < 4)
            {
                RawMagReset[0] += RawExtADCData[8];
                RawMagReset[1] += RawExtADCData[9];
                RawMagReset[2] += RawExtADCData[10];
            }

            MagIndex++;
            if (MagIndex >= 4)
            {
                RawMagData[0] = (RawMagSet[0] - RawMagReset[0]) / 2;
                RawMagData[1] = (RawMagSet[1] - RawMagReset[1]) / 2;
                RawMagData[2] = (RawMagSet[2] - RawMagReset[2]) / 2;

                RawMagSet[0] = 0;
                RawMagSet[1] = 0;
                RawMagSet[2] = 0;

                RawMagReset[0] = 0;
                RawMagReset[1] = 0;
                RawMagReset[2] = 0;

                MagIndex = 0;
            }

            if (Flash_Wr_Offset == 0)
            {
                // start 4
                Flash_Wr_Buf[Flash_Wr_Offset++] = Flash_RawDataConstStartA;
                Flash_Wr_Buf[Flash_Wr_Offset++] = Flash_RawDataConstStartB;
                Flash_Wr_Buf[Flash_Wr_Offset++] = Flash_RawDataConstStartC;
                Flash_Wr_Buf[Flash_Wr_Offset++] = Flash_RawDataConstStartD;

                // PageIndex 4
                // RawExtADCData赋给Flash_Wr_Buf（short->char）
                PageIndex++;
                Flash_Wr_Buf[Flash_Wr_Offset++] = PageIndex & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (PageIndex >> 8) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (PageIndex >> 16) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (PageIndex >> 24) & 0xff;

                // rtc  6
                for (i = 0; i < 6; i++)
                    Flash_Wr_Buf[Flash_Wr_Offset++] = rtc_val[i];
                

                // temp1 2
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[7]) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[7] >> 8) & 0xff;

                // temp2 2
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP >> 8) & 0xff;

                // bat1 2
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP >> 8) & 0xff;

                // bat2 2
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RTC_STAMP >> 8) & 0xff;

                // current 2
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[23]) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[23] >> 8) & 0xff;
                //INC 4
				/*				
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[0]) & 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[0] >> 8)& 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[1])& 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[1] >> 8)& 0xffffff;
								//AZI 4
								Flash_Wr_Buf[Flash_Wr_Offset++] =  IncAzi[2]& 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[2] >> 8)& 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[3])& 0xffffff;
								Flash_Wr_Buf[Flash_Wr_Offset++] = (IncAzi[3] >> 8)& 0xffffff;
*/
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)(StaticToolface*100))& 0xffffff;;  // 静态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)(StaticToolface*100))>> 8);  // 静态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)DynamicToolface*100)& 0xffffff;;  // 动态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)DynamicToolface*100)>> 8);  // 动态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)IncGz*100)& 0xffffff;;  // 井斜
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)IncGz*100)>> 8);  // 井斜
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)AvgInclination*100)& 0xffffff;;  // 方位角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)AvgInclination*100)>> 8);  // 方位角

              
								
                // res 4
                if (sizeof(Flash_Wr_Buf) == 4096)
                    Flash_Wr_Offset = 40;
                else
                    Flash_Wr_Offset = 28;
            }

            // 一条记录  26组，每组2字节
            // RawExtADCData  CH0-6    7
            for (i = 0; i < 7; i++)
            {
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[i]) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[i] >> 8) & 0xff;
            }

            // RawExtADCData  CH8-22  15
            for (i = 8; i < 23; i++)
            {
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[i]) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawExtADCData[i] >> 8) & 0xff;
            }

            // RawIntADCData  CH0-2   3
            for (i = 0; i < 3; i++)
            {
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawIntADCData[i]) & 0xff;
                Flash_Wr_Buf[Flash_Wr_Offset++] = (RawIntADCData[i] >> 8) & 0xff;
            }

            // RawGrammaData       1
            Flash_Wr_Buf[Flash_Wr_Offset++] = (RawGrammaData) & 0xff;
            Flash_Wr_Buf[Flash_Wr_Offset++] = (RawGrammaData >> 8) & 0xff;

            // EventStopA(0);
        }
#endif
        // 每个page满了，存到存储器里
        if (Flash_Wr_Offset >= sizeof(Flash_Wr_Buf))
        {
           
					  mwd2_data_tx.inc = 90;
					  mwd2_data_tx.azi = 160;
					  mwd2_data_tx.update = 1;
            LED_G_ON;
            Storage_APP_Add_MixSensorRaw_Log((uint8_t *)Flash_Wr_Buf, sizeof(Flash_Wr_Buf));//xqj
				    LED_G_OFF;
            
					  bNew_Realtime_RawData = 1; // 提示有新的原始数据
					   short test;
					   for (i = 0; i < 24; i++){
                Realtime_RawData[2*i] = (Realtime_RawData_Temp[i]) & 0xff;							  
                Realtime_RawData[2*i+1] = (Realtime_RawData_Temp[i] >> 8) & 0xff;
							  Realtime_RawData_Temp[i]=0;//复位
               /*
							 test=i;
							 Realtime_RawData[2*i] = (test) & 0xff;					 
               Realtime_RawData[2*i+1] = (test>> 8) & 0xff;*/
            }
						 
     
					  mwd2_data_tx.update = 1;
            Flash_Wr_Offset = 0;
						
             
        }
    }
}

void Creat_user_app_init_task(void)
{
    osThreadNew(user_app_init, NULL, &user_app_init_attr);
}

/*********** (C) COPYRIGHT 2020  skyhigh  *****END OF FILE****/

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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Private define ------------------------------------------------------------*/

// stack size must be multiple of 8 Bytes
#define USER_APP_Init_STK_SZ (5120U)
#define WINDOW_SIZE 100
#define PI 3.14159265358979323846f
#define INF 100000;
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

volatile unsigned char Realtime_RawData[48]; //存一组实时数据
volatile short Realtime_RawData_Temp[24]={0};//用于计算实时数据的均值



// 姿态角度
float GyroRotation=0;    // 陀螺转速
float StaticToolface=0;  // 静态工具面角
float DynamicToolface=0; // 动态工具面角
float Inc=0;           // 井斜
float Azi=0;           //方位

//以下参数用于滑动平均滤波
float CurrentAzi;
float CurrentInc;
float CurrentGyro;
volatile float AziHistory[WINDOW_SIZE]={0};
volatile float IncHistory[WINDOW_SIZE]={0};
volatile float GyroHistory[WINDOW_SIZE]={0};
float AziHistorySum=0;
float IncHistorySum=0;
float GyroHistorySum=0;
float AziOld=0;
float IncOld=0;
float GyroOld=0;
short AziIndex=0;
short IncIndex=0;
short GyroIndex=0;
short flag=0;
//地磁场:总场、地磁倾角、地磁偏角
//const float B=50000.0f;
//const float GeoI=-30.0f/180.0f*PI;
//const float GeoD=180.0f/180.0f*PI;

const float B=53241.20f;
const float GeoI=56.2f/180.0f*PI;
const float GeoD=-7.66f/180.0f*PI;

//const float B=55116.5f;
//const float GeoI=59.83f/180.0f*PI;
//const float GeoD=-7.95f/180.0f*PI;
//Bu=B*sin(GeoI)
//Bn=B*cos(GeoI)*cos(GeoD);
//Be=B*cos(GeoI)*sin(GeoD);

float Bu=4424.26f;
float Bn=2935.35f;
float Be=-3947.89f;

//float Bu=-25000.0f;
//float Bn=-43300.02f;
//float Be=0.0f;

//float Bu=47650.28f;
//float Bn=27433.58f;
//float Be=-3831.12f;
//用于粒子群
const float e1 = 10000;
const float e3 =10e-8;

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
//原始值均值
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
//滑动平均滤波
float slidingAverageFilter(float newMeasurement, float *historysum, short *index, float oldMeasurement,short flag) {
    *historysum += newMeasurement-oldMeasurement;
    *index = (*index + 1) % WINDOW_SIZE;
    return *historysum/flag;
}

//Mz理论值
float MzTheory(float EA, float EI,float cbz)
{
    return -cos(EA) * sin(EI) * Bn + sin(EA) * sin(EI) * Be - cos(EI) * Bu - cbz;
}

//适应性函数，判断估计值EA,EI的可信度，el为阈值
float Fitness(float EA, float EI,float cbz, double e1)
{
    float bz;
    bz = fabs(MzTheory(EA, EI,cbz));
    if (bz > e1)
    {
      return INF;
    }else{
    return bz;
    }
}
//判断参数是否处于范围
float OnRange(float pos,float rangemin, float rangemax)
{
    if (pos<rangemin)
    {
        return rangemin;
    }else if(pos>rangemax)
    {
        return rangemax;
    }else
    {
        return pos;
    }
}

//取数组最小值，并返回位置
float* findmin(float array[],int arraylength)
{
    float MinandIndex[2]={array[0],0};
    float* ans;
    for (short i=1;i<arraylength;i++)
    {
         if (array[i]<MinandIndex[0])
         {
            MinandIndex[0]=array[i];
            MinandIndex[1]=i;
         }
    }
    ans=&MinandIndex[0];
    return ans;
}

//PSO解算方位角和井斜角
void PSO(){

	
	  //范围限制
   float alimit[2] = {(Azi-1),(Azi+1)};
   float ilimit[2] = {(Inc-1.5),(Inc+1)};
		//float alimit[2] = {(40.0),(40.0)};
   // float ilimit[2] = {(44.1),(46.2)};
    // 速度限制
    float avlimit[2] = {-0.5, 0.5};
    float ivlimit[2] = {-0.5, 0.5};
		short ger = 50;
    // 种群数量
    short NumOfBird = 20;
		// 粒子初始速度与位置
    float birdapos[NumOfBird];
    float birdipos[NumOfBird];
  
    float birdav[NumOfBird];
    float birdiv[NumOfBird];
    // 粒子位置、速度初始化
    // 固定粒子
    birdapos[0] = alimit[0];
    birdipos[0] = ilimit[0];
    birdapos[1] = alimit[1];
    birdipos[1] = ilimit[0];
    birdapos[2] = alimit[0];
    birdipos[2] = ilimit[1];
    birdapos[3] = alimit[1];
    birdipos[3] = ilimit[1];
    birdapos[4] = alimit[0];
    birdipos[4] = ilimit[0];
    birdapos[5] = alimit[1];
    birdipos[5] = ilimit[1];
    birdapos[6] = alimit[0];
    birdipos[6] = ilimit[1];
    birdapos[7] = alimit[1];
    birdipos[7] = ilimit[0];
    birdapos[8] = (alimit[0] + alimit[1]) / 2;
    birdipos[8] = (ilimit[0] + ilimit[1]) / 2;
	
		
		//随机粒子
		// srand((unsigned)time(NULL)); 本系统不支持，无语
		srand(12345);
	
    for (short i=9;i<NumOfBird;i++)
    {
        birdapos[i]= alimit[0] + 1.0 * (rand() % RAND_MAX) / RAND_MAX * (alimit[1] - alimit[0]);
        birdipos[i]= ilimit[0] + 1.0 * (rand() % RAND_MAX) / RAND_MAX * (ilimit[1] - ilimit[0]);
    }
    //速度初始化
    for (short i=0;i<NumOfBird;i++)
    {
        birdav[i]= avlimit[0] + 1.0 * (rand() % RAND_MAX) / RAND_MAX * (avlimit[1] - avlimit[0]);
        birdiv[i]= ivlimit[0] + 1.0 * (rand() % RAND_MAX) / RAND_MAX * (ivlimit[1] - ivlimit[0]);
    }
		
	

		
		//权值
    float w = 0.2;
    float c1 = 0.2;
    float c2 = 0.8;
		//个体最优适应度、位置
    float pbestfitness[NumOfBird];
    float pbesta[NumOfBird];
    float pbesti[NumOfBird];
    for (short i=0;i<NumOfBird;i++)
    {
        pbestfitness[i]=INF;
    }
		//群体最优适应度、位置
    float gbestfitness=INF;
    float gbesta;
    float gbesti;
    
    //每次迭代粒子适应度
    float birdfitness[NumOfBird];
    short t=0;
    short index;
    float* temp;
		//粒子群算法测试，模拟一组测量值
		
		//float testBz=MzTheory(40.0f/180.0f*PI, 45.0f/180.0f*PI,0);  
		float testBz=fMagData.MagZ;
		while(t<ger)
    {   
        //第t次迭代各粒子的适应度
        for (short i=0; i<NumOfBird; i++)
        {   
          // birdfitness[i]=Fitness(birdapos[i]/180.0*PI, birdipos[i]/180.0 * PI,fMagData.MagZ,e1);
					 birdfitness[i]=Fitness(birdapos[i]/180.0*PI, birdipos[i]/180.0 * PI,testBz,e1);
        }
        //更新群体最优适应度以及位置
        temp=findmin(birdfitness,NumOfBird);
        if (temp[0]<gbestfitness)
        {
            gbestfitness=temp[0];
            index=temp[1];
            gbesta=birdapos[index];
            gbesti=birdipos[index];                  
        }

        //判断是否达到截止阈值
        if (gbestfitness<e3)
        {
            break;
        }

        //更新个体最优适应度、位置
        for (short i=0; i<NumOfBird; i++)
        {
            if (birdfitness[i]<pbestfitness[i])
            {
                pbestfitness[i]=birdfitness[i];
                pbesta[i]=birdapos[i];
                pbesti[i]=birdipos[i];
              
            }
        }

        //更新粒子速度
        for (short i=0; i<NumOfBird;i++)
        {
            birdav[i]=birdav[i]*w+0.5*(rand()/RAND_MAX+1.0)*(pbesta[i]-birdapos[i])+0.5*(rand()/RAND_MAX+1.0)*(gbesta-birdapos[i]);
            birdav[i]=OnRange(birdav[i],avlimit[0], avlimit[1]);
            birdiv[i]=birdav[i]*w+0.5*(rand()/RAND_MAX+1.0)*(pbesti[i]-birdipos[i])+0.5*(rand()/RAND_MAX+1.0)*(gbesti-birdipos[i]);
            birdiv[i]=OnRange(birdiv[i],ivlimit[0], ivlimit[1]);  
                    
        }

        //更新粒子位置
        for (short i=0;i<NumOfBird; i++)
        {
            birdapos[i]+=birdav[i];
            birdapos[i]=OnRange(birdapos[i],alimit[0], alimit[1]);
            birdipos[i]+=birdiv[i];
            birdipos[i]=OnRange(birdipos[i],ilimit[0], ilimit[1]);
        }

        t++;
    }
	  CurrentAzi=gbesta;
	  CurrentInc=gbesti;

}

//归一化
void Normalization(float *Mx,float *My,float *Mz,float TotalB){
	*Mx=*Mx/TotalB;
	*My=*My/TotalB;
	*Mz=*Mz/TotalB;
}


/**
 * @brief 把外部ADC的原始值转换成未校正的实际值
 *
 */
void ConvertExtADC(void)
{   // 直接将校正值赋给RawExtADCData
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
    // HR1_Y  356B_1_Z y加速度1
    ftmp = RawExtADCData[1];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRPRef) / 80.0f;
    fDiffAccData.AccHyP = ftmp;
    //fDiffAccData.AccHyP =  ftmp;//xqj
    // HR1_Z  -356B_1_Y z加速度1
    ftmp = RawExtADCData[2];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRPRef) / 80.0f;
    // fDiffAccData.AccHzP = 0 - ftmp;
    fDiffAccData.AccHzP = 0-ftmp;//xqj
    // HR2_X  -356B_2_Y x加速度2
    ftmp = RawExtADCData[3];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRNRef) / 80.0f;
    fDiffAccData.AccHxN = 0 - ftmp;
    //fDiffAccData.AccHxN =  ftmp;
    // HR2_Y  -356B_2_Z y加速度2
    ftmp = RawExtADCData[4];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_A_Ref + AccSensorRef.ADC_A_Ref - AccSensorRef.AccHRNRef) / 80.0f;
    fDiffAccData.AccHyN = 0- ftmp;
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
    fMagData.MagZ = ftmp;
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
    fDiffAccData.AccLxP = ftmp;
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
    fDiffAccData.AccLxN = 0-ftmp;
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
    //

    // LR 45//////////////////////////////////////////////////////////////////////////////
    //////     45
    //////////////////////////////////////////////////////////////////////////////////////
    // LR3_X 354_3_Z
    ftmp = RawExtADCData[17];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45xP = ftmp;
    // LR3_Y*  354_3_Y
    ftmp = RawExtADCData[18];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45yP = ftmp;
    // LR3_Z*  354_3_X
    ftmp = RawExtADCData[19];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45PRef) / 400.0f;
    fDiffAccData.AccL45zP = ftmp;
    //
    fDiffAccData.AccL45yP = fDiffAccData.AccL45yP*0.707107f - fDiffAccData.AccL45zP*0.707107f;
    fDiffAccData.AccL45zP = - (fDiffAccData.AccL45yP + fDiffAccData.AccL45zP)*0.707107f;
    // LR 45//////////////////////////////////////////////////////////////////////////////
    //////     45
    //////////////////////////////////////////////////////////////////////////////////////
    // LR4_X  354_4_Z
    ftmp = RawExtADCData[20];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45xN = -ftmp;
    // LR4_Y*  354_4_X
    ftmp = RawExtADCData[21];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45yN = ftmp;
    // LR4_Z*  354_4_Y
    ftmp = RawExtADCData[22];
    ftmp = (ftmp / 32768.0f * AccSensorRef.ADC_B_Ref + AccSensorRef.ADC_B_Ref - AccSensorRef.AccLR45NRef) / 400.0f;
    fDiffAccData.AccL45zN = ftmp;

    fDiffAccData.AccL45yN = fDiffAccData.AccL45zN*0.707107f - fDiffAccData.AccL45yN*0.707107f;
    fDiffAccData.AccL45zN = (fDiffAccData.AccL45zN + fDiffAccData.AccL45yN)*0.707107f;
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
    //Init_MWD_APP();
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
					  //给原始值取均值
            AvgOfRawData();   
            // 数据转换
            ConvertExtADC();
					  // 磁数据归一化
				  	//Normalization(&fMagData.MagX,&fMagData.MagY,&fMagData.MagZ,B);
            // 数据校正：
            gyroRotation(&fRotate,fTemperature); // 陀螺转速校正
				  	//加速度计校正，这里使用AccLP加速度组，即第三组，其他组暂时用不用
					  AccCalibrate(&fDiffAccData.AccLxP,&fDiffAccData.AccLyP,&fDiffAccData.AccLzP,fTemperature,3);
					  //磁校正
					  MagCalibrate(&fMagData.MagX,&fMagData.MagY,&fMagData.MagZ,fTemperature);
					

					  //姿态角解算
					  if (fRotate<20.0f&&fRotate>-20.0f){
								//静态
							StaticToolface=GravityToolface(fDiffAccData.AccLxP,fDiffAccData.AccLyP);
							DynamicToolface=StaticToolface;
							CurrentInc=calculateInclination(fDiffAccData.AccLxP,fDiffAccData.AccLyP,-fDiffAccData.AccLzP);					  
							CurrentAzi=calculateAzi(CurrentInc,Be,Bn,Bu,fMagData.MagZ,GeoD,CurrentAzi);			
						
							  //测试一下粒子群算法
							  //预设井斜角方位角
							// Inc=45.0;
							// Azi=40.0;
							// CurrentInc=45.0;
							// CurrentAzi=45.0;
							// CurrentAzi=MzTheory(40.0f/180.0f*PI, 45.0f/180.0f*PI,0);
							  // PSO();
							//
						}else{
						    //动态
						
						  DynamicToolface+=fRotate/50.0f;
							if(DynamicToolface>360.0f) DynamicToolface-=360;
						  PSO();									
						}
						//姿态解算结束
						
						//滑动平均
						
						if(flag<WINDOW_SIZE){
								//测量值数量小于窗口长度
								flag++;
								AziOld=0;
								IncOld=0;
												
						}else{
							AziOld=AziHistory[AziIndex];
              IncOld=IncHistory[IncIndex];
										
						}  
						AziHistory[AziIndex]=CurrentAzi;
						IncHistory[IncIndex]=CurrentInc;
						Azi=slidingAverageFilter(CurrentAzi,&AziHistorySum,&AziIndex, AziOld,flag);
						Inc=slidingAverageFilter(CurrentInc,&IncHistorySum,&IncIndex, IncOld,flag);

				    //滑动平均结束
						
						//上传QB
					  mwd2_data_tx.inc = Inc;
					  mwd2_data_tx.azi = Azi;
					  mwd2_data_tx.update = 1;
            LED_R_OFF;
						
						//splitFloat(mwd2_data_tx.inc, &RawExtADCData[21], &RawExtADCData[22]);
						//splitFloat(mwd2_data_tx.azi, &RawExtADCData[21], &RawExtADCData[22]);

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
								
                //姿态角存入包头：？
				Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)Inc*100)& 0xff;  // 井斜
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)Inc*100)>> 8);  // 井斜
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)Azi*100)& 0xff;  // 方位角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)Azi*100)>> 8);  // 方位角			
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)(StaticToolface*100))& 0xff;  // 静态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)(StaticToolface*100))>> 8);  // 静态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = ((short)DynamicToolface*100)& 0xfff;  // 动态工具面角
                Flash_Wr_Buf[Flash_Wr_Offset++] = (((short)DynamicToolface*100)>> 8);  // 动态工具面角
            
							
                //for git test,view
								
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
           
					  //mwd2_data_tx.inc = Inc;
					 // mwd2_data_tx.azi = Azi;
					  //mwd2_data_tx.update = 1;
            LED_G_ON;
            Storage_APP_Add_MixSensorRaw_Log((uint8_t *)Flash_Wr_Buf, sizeof(Flash_Wr_Buf));//xqj
				    LED_G_OFF;
            
					   bNew_Realtime_RawData = 1; // 提示有新的原始数据
					
				     Realtime_RawData_Temp[20]=0;//非常玄学的一行，不加这一行后面赋值都不行:借用他的位置上传实时姿态角
					   Realtime_RawData_Temp[20]=(short)(Inc*100);
					   Realtime_RawData_Temp[21]=(short)(Azi*100);
             Realtime_RawData_Temp[22]=(short)(StaticToolface*100);
					
 					   for (i = 0; i < 24; i++){
                Realtime_RawData[2*i] = (Realtime_RawData_Temp[i]) & 0xff;							  
                Realtime_RawData[2*i+1] = (Realtime_RawData_Temp[i] >> 8) & 0xff;
							  Realtime_RawData_Temp[i]=0;//复位
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

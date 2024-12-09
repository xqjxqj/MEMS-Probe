#ifndef _User_storage_h_
#define _User_storage_h_

#include "main.h"
 

#if 0
 
#else
#define RecordFileNameSize 10            // 单个文件名长度10字节 6.3
#define OneRecordFileNameBufferLength 11 // 单个文件名的字符串存储空间长度
#endif



#define MaxAnalysisExtantRecordFiles 10 //  挑选出最近处理的SRR文件 的最大数量

#define MaxRecordFile 200 // 最多允许存储的记录文件数


// 存储器状态
typedef enum
{
	StorageStatus_NotPresent = 0,
	StorageStatus_OK,
	StorageStatus_Full,
	StorageStatus_Error
	//StorageStatus_WRError=0x80
}StorageStatus_TypeDef;



// 记录目前已有多少个记录文件
extern unsigned short RecordFileCount;
 

extern uint64_t CardFreeCapacity;
extern StorageStatus_TypeDef Storage_Status;
                             
extern void Storage_SetfsDateTime(unsigned char *_datetime);
extern void Storage_GetFreeCardCapacity(char *Drivename);

extern uint32_t Storage_Init(char *Drivename);
extern uint32_t Storage_Format(char *Drivename);
extern uint32_t Storage_ReadFile(const char *ReadFileName, uint8_t *ReadoutBuf, uint32_t MaxReadoutLen);
extern uint32_t Storage_ReadFileContinue(const char *ReadFileName, uint8_t *ReadoutBuf, uint32_t MaxReadoutLen, uint32_t RdStartPostion);
extern uint32_t Storage_WriteNewFile(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen);
extern uint32_t Storage_WriteFile_Append(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen);
extern uint32_t Storage_WriteFile_Insert(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen, uint32_t WrStartPostion);
extern uint32_t Storage_DelectFile(const char *DelectFileName);

extern uint32_t Storage_APP_Add_GroundCMD_Log(uint8_t *GroundCMD_Log, uint32_t size);

extern uint32_t Storage_APP_Add_MixSensorRaw_Log(uint8_t *SensorRaw_Log, uint32_t size);

extern uint32_t Storage_APP_Delete_GroundCMD_Log(void);

extern uint32_t Storage_APP_Delete_MixSensorRaw_Log(void);
extern uint32_t Storage_APP_GetSize_MixSensorRaw_Log(void);
extern uint32_t Storage_APP_GetData_MixSensorRaw_Log(uint8_t *pRead,uint32_t MaxReadoutLen);

#endif

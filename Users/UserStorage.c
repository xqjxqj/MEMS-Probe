/* Includes ------------------------------------------------------------------*/
#include "UserStorage.h"
// #include "background.h"

#include <stdio.h>	// Standard I/O .h-file
#include <ctype.h>	// Character functions
#include <string.h> // String and memory functions

#include "main.h"

#include "rl_fs.h" // Keil.MDK-Pro::File System:CORE

#include "fsmc_nand.h"
#include "FS_Config_NAND_0.h"

// 存储卡总容量
long CardCapacity;

// 存储卡空闲容量
uint64_t CardFreeCapacity;

// 存储器状态
StorageStatus_TypeDef Storage_Status;

// 供文件系统使用的时间，可用于修改文件的创建、写入时间
fsTime fsDateTime;

// 记录目前已有多少个记录文件
unsigned short RecordFileCount;

char fn_cmd[] = "cmd.rcl";
char fn_MixSensor[] = "sensor.mwr"; // mems wellline rawdata
//char fn_MixSensor_mask[] = "sensor.msr"; // mix sensor raw
char fn_MixSensor_mask[] = "*.mwr"; // mix raw data

int32_t Driver_NAND0_GetDeviceBusy(uint32_t dev_num)
{
	//  uint16_t i;

	//  while ((GPIOD->IDR & FLASH_RnB_Pin) == 0x00)
	//  {
	//    for (i = 200; i > 0; i--)
	//      ;
	//  }

	//  for (i = 200; i > 0; i--)
	//    ;

	uint16_t k;
	for (k = 0; k < 200; k++)
	{
		if ((GPIOD->IDR & FLASH_RnB_Pin) == 0)
			break;
	}
	for (k = 0; k < 2000; k++)
	{
		if ((GPIOD->IDR & FLASH_RnB_Pin) != 0)
			break;
	}

	return 0x00;
}

/*
 * @brief  设定文件系统的时间
 * @param  _datetime：依次是年（实际值-2000）月日时分秒
 * @retval  no
 * */
void Storage_SetfsDateTime(unsigned char *_datetime)
{
	fsDateTime.year = (*_datetime++) + 2000; // 2019;
	fsDateTime.mon = (*_datetime++);		 // 10;
	fsDateTime.day = (*_datetime++);		 // 25;

	fsDateTime.hr = (*_datetime++);	 // 17;
	fsDateTime.min = (*_datetime++); // 30;
	fsDateTime.sec = (*_datetime++); // 2;
}

/*
 * @brief  获取可用存储容量
 * @param  Drivename: 驱动器名，如"M0:"
 * @retval  no
 * */
void Storage_GetFreeCardCapacity(char *Drivename)
{
	CardFreeCapacity = ffree(Drivename);
}

/*
 * @brief  初始化
 * @param  Drivename: 驱动器名，如"M0:"
 * @retval  0=成功，1=失败
 * */
uint32_t Storage_Init(char *Drivename)
{
	int i;
	
	Storage_Status = StorageStatus_NotPresent;

	fsStatus stat;

	// if (MCI_ReadCD(0) == 0)
	// 	return 1;

	/* Initialize and mount drive "M0" */
	stat = finit(Drivename);
	//擦除XQJ
	/*
	for(i=0;i<NAND0_BLOCK_COUNT;i++)
				FlashEraseBlock(i);
			
			stat = fformat(Drivename, "/L WellLine /FAT32");
	*/
	///////////////////////////
	if (stat == fsOK)
	{
		stat = fmount(Drivename);
		if (stat == fsOK)
		{
			Storage_GetFreeCardCapacity(Drivename);
			Storage_Status = StorageStatus_OK;
			return 0;
		}
		else if (stat == fsNoFileSystem)
		{
			stat = fformat(Drivename, "/L WellLine /FAT32");
			if ( stat == fsOK)
			{
				Storage_GetFreeCardCapacity(Drivename);
				
				Storage_Status = StorageStatus_OK;
				return 0;
			}
		}
		else
		{
			for(i=0;i<NAND0_BLOCK_COUNT;i++)
				FlashEraseBlock(i);
			
			stat = fformat(Drivename, "/L WellLine /FAT32");
			if ( stat == fsOK)
			{
				Storage_GetFreeCardCapacity(Drivename);
				
				Storage_Status = StorageStatus_OK;
				return 0;
			}
			
			//// printf ("Drive M0 mount failed with error code %d\n", stat);
		}
	}
	else
	{
		//// printf ("Drive M0 initialization failed!\n");
	}
	return 1;
}

/*
 * @brief  格式化
 * @param  Drivename: 驱动器名，如"M0:"
 * @retval  0=成功，1=失败
 * */
uint32_t Storage_Format(char *Drivename)
{
	Storage_Status = StorageStatus_NotPresent;

	fsStatus stat;

	// if (MCI_ReadCD(0) == 0)
	// 	return 1;

	/* Initialize and mount drive "M0" */
	stat = finit(Drivename);
	if (stat == fsOK)
	{
		stat = fmount(Drivename);
		if (stat == fsOK)
		{
			if (fformat(Drivename, "/L IGG_MEMS_WellLine /FAT32") == fsOK)
			{
				Storage_Status = StorageStatus_OK;
				return 0;
			}

			Storage_Status = StorageStatus_OK;
			return 0;
		}
		else if (stat == fsNoFileSystem)
		{
			if (fformat(Drivename, "/L IGG_MEMS_WellLine /FAT32") == fsOK)
			{
				Storage_Status = StorageStatus_OK;
				return 0;
			}
		}
		else
		{
			//// printf ("Drive M0 mount failed with error code %d\n", stat);
		}
	}
	else
	{
		//// printf ("Drive M0 initialization failed!\n");
	}
	return 1;
}

/*
 * @brief  从文件中读出一定长度的数据
 * @param  ReadFileName: 文件名
 * @param  ReadoutBuf: 存放读出的数据
 * @param  MaxReadoutLen:数据长度
 * @param  RdStartPostion:起始读出位置
 * @retval  实际读出的数据长度
 * */
uint32_t Storage_ReadFile(const char *ReadFileName, uint8_t *ReadoutBuf, uint32_t MaxReadoutLen)
{
	uint32_t size = 0, BytesRead;

	FILE *fin;
	fin = fopen(ReadFileName, "r");
	if (fin == NULL)
	{
		Storage_Status = StorageStatus_Error;
		// fclose(fin);
		return 0;
	}

	BytesRead = fread(ReadoutBuf, 1, MaxReadoutLen, fin);
	fclose(fin);
	Storage_Status = StorageStatus_OK;
	return BytesRead;
}

/*
 * @brief  文件的指定位置读入数据
 * @param  ReadFileName: 文件名
 * @param  ReadoutBuf: 存放读出的数据
 * @param  MaxReadoutLen:数据长度
 * @param  RdStartPostion:起始读出位置
 * @retval  实际读出的数据长度
 * */
uint32_t Storage_ReadFileContinue(const char *ReadFileName, uint8_t *ReadoutBuf, uint32_t MaxReadoutLen, uint32_t RdStartPostion)
{
	uint32_t size = 0, BytesRead;

	FILE *fin;
	fin = fopen(ReadFileName, "r");
	if (fin == NULL)
	{
		Storage_Status = StorageStatus_Error;
		// fclose(fin);
		return 0;
	}

	if (fseek(fin, RdStartPostion, SEEK_SET) == 0) //  SEEK_SET = begiin of the file
	{
		BytesRead = fread(ReadoutBuf, 1, MaxReadoutLen, fin);
	}
	fclose(fin);
	Storage_Status = StorageStatus_OK;
	return BytesRead;
}
/*
 * @brief  新建一个文件，并写入内容
 * @param  WriteFileName: 文件名
 * @param  WriteInBuf: 要写入的数据
 * @param  MaxWriteInLen:数据长度
 * @retval  实际写入的数据长度
 * */
uint32_t Storage_WriteNewFile(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen)
{
	uint32_t size = 0;
	FILE *fout;
	Storage_Status = StorageStatus_Error;

	fout = fopen(WriteFileName, "w");
	if (fout == NULL)
	{
		return 0;
	}

	size = fwrite(WriteInBuf, sizeof(uint8_t), MaxWriteInLen, fout);
	if (size == MaxWriteInLen)
	{
		Storage_Status = StorageStatus_OK;
	}
	fclose(fout);

	ftime_set(WriteFileName, &fsDateTime, NULL, NULL); //  filename,create,access,write
	return size;
}

/**
 * @brief  在文件末尾添加数据
 * @param  WriteFileName: 文件名
 * @param  WriteInBuf: 要写入的数据
 * @param  MaxWriteInLen:数据长度
 * @retval  实际写入的数据长度
 */
uint32_t Storage_WriteFile_Append(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen)
{
	uint32_t size = 0;
	FILE *fout;

	Storage_Status = StorageStatus_Error;

	fout = fopen(WriteFileName, "a"); // a  文件不存在的话就自动建立
	if (fout == NULL)
	{
		// fout = fopen(WriteFileName, "bw"); // a  文件不存在的话就自动建立
		// if (fout == NULL)
		return 0;
	}

	// size=fseek(fout,3, SEEK_SET);
	size = fwrite(WriteInBuf, sizeof(uint8_t), MaxWriteInLen, fout);
	if (size == MaxWriteInLen)
	{
		Storage_Status = StorageStatus_OK;
	}
	fclose(fout);
	ftime_set(WriteFileName, NULL, NULL, &fsDateTime); //  filename,create,access,write
	return size;
}

/**
 * @brief  在文件中某个位置开始覆盖一段数据
 * @param  WriteFileName: 文件名
 * @param  WriteInBuf: 要写入的数据
 * @param  MaxWriteInLen:数据长度
 * @retval  实际写入的数据长度
 */
uint32_t Storage_WriteFile_Insert(const char *WriteFileName, uint8_t *WriteInBuf, uint32_t MaxWriteInLen, uint32_t WrStartPostion)
{
	uint32_t size = 0;
	FILE *fout;

	Storage_Status = StorageStatus_Error;

	fout = fopen(WriteFileName, "rb+");
	if (fout == NULL)
	{
		return 0;
	}

	size = fseek(fout, WrStartPostion, SEEK_SET);
	size = fwrite(WriteInBuf, sizeof(uint8_t), MaxWriteInLen, fout);
	if (size == MaxWriteInLen)
	{
		Storage_Status = StorageStatus_OK;
	}

	fclose(fout);
	ftime_set(WriteFileName, NULL, NULL, &fsDateTime); //  filename,create,access,write

	return size;
}
/**
 * @brief   删除文件
 * @param  DelectFileName: 文件名
 * @retval  0=失败，1=成功
 */
uint32_t Storage_DelectFile(const char *DelectFileName)
{
	if (fdelete(DelectFileName, NULL) == fsOK)
	{ // Delete a file from current drive.
		return 1;
	}
	return 0;
}


/**
 * @brief   删除0字节文件, 返回成功删除文件的数量
 * @param  mask为寻找文件的匹配条件，如"*.IPS" 查找所有IPS类文件
 * @param  bWithDir=1 下级子文件夹也搜索
 * @retval  删除文件的数量
 */
uint32_t Storage_DelectZeroRecordFile(char *mask, uint8_t bWithDir)
{
	uint64_t fsize;
	uint32_t files, dirs, i;
	uint32_t delectedfiles;
	char ch;
	fsFileInfo info;

	// mask = "*.IPS";

	files = 0;
	delectedfiles = 0;
	dirs = 0;
	fsize = 0;
	info.fileID = 0;
	while (ffind(mask, &info) == fsOK)
	{
		if (info.attrib & FS_FAT_ATTR_DIRECTORY) // 文件夹
		{
			if (bWithDir)
			{
				i = 0;
				while (strlen((const char *)info.name + i) > 41)
				{
					ch = info.name[i + 41];
					info.name[i + 41] = 0;
					// printf ("\n%-41s", &info.name[i]);
					info.name[i + 41] = ch;
					i += 41;
				}
				// printf ("\n%-41s    <DIR>       ", &info.name[i]);
				// printf ("  %02d.%02d.%04d  %02d:%02d",info.time.day, info.time.mon, info.time.year,info.time.hr, info.time.min);

				dirs++;

				if (info.size == 0) // 文件大小为0
					if (fdelete(&info.name[i], NULL) == fsOK)
					{
						delectedfiles++;
						// return delectedfiles;
					}
			}
		}
		else
		{
			i = 0;
			while (strlen((const char *)info.name + i) > 41)
			{
				ch = info.name[i + 41];
				info.name[i + 41] = 0;
				// printf ("\n%-41s", &info.name[i]);

				info.name[i + 41] = ch;
				i += 41;
			}
			// printf ("\n%-41s %14s ", &info.name[i], temp);
			// printf ("  %02d.%02d.%04d  %02d:%02d",info.time.day, info.time.mon, info.time.year,info.time.hr, info.time.min);
			fsize += info.size;
			files++;

			if (info.size == 0) // 文件大小为0
				if (fdelete(&info.name[i], NULL) == fsOK)
				{ // Delete a file from current drive.
					delectedfiles++;
					// return delectedfiles;
				}
		}
	}

	return delectedfiles;
}

/**
 * @brief  删除匹配文件名的文件，返回成功删除文件的 数量
 * @param  mask 文件名匹配条件
 * @param  bWithDir =1 下级子文件夹也搜索
 * @return uint32_t
 */
static uint32_t Storage_Delect_MaskedRecordFile(char *mask, uint8_t bWithDir)
{
	uint64_t fsize;
	uint32_t files, dirs, i;
	uint32_t delectedfiles;
	char ch;
	fsFileInfo info;

	// mask = "*.IPS";

	files = 0;
	delectedfiles = 0;
	dirs = 0;
	fsize = 0;
	info.fileID = 0;
	while (ffind(mask, &info) == fsOK)
	{
		if (info.attrib & FS_FAT_ATTR_DIRECTORY) // 文件夹
		{
			if (bWithDir)
			{
				i = 0;
				while (strlen((const char *)info.name + i) > 41)
				{
					ch = info.name[i + 41];
					info.name[i + 41] = 0;
					// printf ("\n%-41s", &info.name[i]);
					info.name[i + 41] = ch;
					i += 41;
				}
				// printf ("\n%-41s    <DIR>       ", &info.name[i]);
				// printf ("  %02d.%02d.%04d  %02d:%02d",info.time.day, info.time.mon, info.time.year,info.time.hr, info.time.min);

				dirs++;

				if (fdelete(&info.name[i], NULL) == fsOK)
				{
					delectedfiles++;
					// return delectedfiles;
				}
			}
		}
		else
		{
			i = 0;
			while (strlen((const char *)info.name + i) > 41)
			{
				ch = info.name[i + 41];
				info.name[i + 41] = 0;
				// printf ("\n%-41s", &info.name[i]);

				info.name[i + 41] = ch;
				i += 41;
			}
			// printf ("\n%-41s %14s ", &info.name[i], temp);
			// printf ("  %02d.%02d.%04d  %02d:%02d",info.time.day, info.time.mon, info.time.year,info.time.hr, info.time.min);
			fsize += info.size;
			files++;

			if (fdelete(&info.name[i], NULL) == fsOK)
			{ // Delete a file from current drive.
				delectedfiles++;
				// return delectedfiles;
			}
		}
	}

	return delectedfiles;
}


/**
 * @brief 获取某个文件的大小
 * 
 * @param filename 
 * @return uint32_t 
 */
uint32_t Storage_GetFileSize(char *filename)
{
	fsFileInfo info;
	fsStatus fs;
	
	//fs = ffind(fn_MixSensor_mask, &info);
	fs = ffind(filename, &info);
	if ( fs == fsOK)	 
		 	return  (uint32_t)(info.size);		 
	else
		return  (uint32_t)(0xFFFFFFFF);	 
}

//---------------------- 以下结合实际作业文件特性而用的子函数 --------------

/**
 * @brief  删除 错误文件，如文件长度为0，文件名中含有~
 * @return uint32_t 删除文件的数量
 */
uint32_t Storage_APP_Delect_Invalid_RecordFiles(void)
{
	uint32_t cnt = 0;

	return cnt;
}

/**
 * @brief  删除所有记录文件
 * @return uint32_t 删除文件的数量
 */
uint32_t Storage_APP_Delect_All_RecordFiles(void)
{
	uint32_t cnt = 0;
	// 删除所有匹配关系的文件
	// cnt += Storage_Delect_MaskedRecordFile("*.SRR", 0);
	// cnt += Storage_Delect_MaskedRecordFile("*.IPS", 0);
	// cnt += Storage_Delect_MaskedRecordFile("*.REC", 0);
	return cnt;
}

/**
 * @brief  删除地面命令log文件
 * @return uint32_t
 */
uint32_t Storage_APP_Delete_GroundCMD_Log(void)
{
	return Storage_DelectFile(fn_cmd);
}

/**
 * @brief  增加地面命令log文件内容
 * @param  GroundCMD_Log    一条地面命令内容
 * @param  size             一条地面命令长度（字节）
 * @return uint32_t
 */
uint32_t Storage_APP_Add_GroundCMD_Log(uint8_t *GroundCMD_Log, uint32_t size)
{
	//if (Storage_Status == StorageStatus_OK)
	{
		return Storage_WriteFile_Append(fn_cmd, GroundCMD_Log, size);
	}
	return 0;
}

/**
 * @brief  删除传感器原始数据存储文件
 * @return uint32_t
 */
uint32_t Storage_APP_Delete_MixSensorRaw_Log(void)
{
	return Storage_DelectFile(fn_MixSensor);
}

/**
 * @brief  增加传感器原始数据存储
 * @param  SensorRaw_Log    一条传感器记录内容
 * @param  size             一条传感器记录 长度（字节）
 * @return uint32_t
 */
uint32_t Storage_APP_Add_MixSensorRaw_Log(uint8_t *SensorRaw_Log, uint32_t size)
{
	if (Power_off == 0)
	{
		return Storage_WriteFile_Append(fn_MixSensor, SensorRaw_Log, size);
	}
	return 0;
}

/**
 * @brief  获取传感器原始数据存储大小
 * 
 * @return uint32_t 
 */
uint32_t Storage_APP_GetSize_MixSensorRaw_Log(void)
{
	return Storage_GetFileSize(fn_MixSensor);
}


/**
 * @brief 获取传感器原始数据
 * 
 * @param pRead  读取数据的缓冲区
 * @param MaxReadoutLen  期望最大读出数据字节数
 * @return uint32_t    实际读出数据字节数
 */
uint32_t Storage_APP_GetData_MixSensorRaw_Log(uint8_t *pRead,uint32_t MaxReadoutLen)
{
	return Storage_ReadFile(fn_MixSensor, pRead,  MaxReadoutLen);	
}
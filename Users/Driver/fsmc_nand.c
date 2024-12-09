/***************************************************************

    程 序 名：NAND FLASH读写程序
    公    司：
    作    者：
	版 本 号：
    建立日期：
    修改日期：
    编译环境： KEIL FOR ARM
    硬件平台：  K9F1G08U0A大页NAND
    功能描述： 扇区读写，带擦除管理

	注：该程序移植于网友电脑圈圈，仅对底层的读写函数做了修改。

***************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include <stdio.h>
#include "fsmc_nand.h"

#include "FS_Config_NAND_0.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#if 1
    // APP board
//	#define CMD_AREA                   (u32)(1<<16)   /* A16 = CLE  high */
//	#define ADDR_AREA                  (u32)(1<<17)  /* A17 = ALE high */
#else
    // develop board
	#define CMD_AREA                   (u32)(1<<17)   /* A17 = CLE  high */
	#define ADDR_AREA                  (u32)(1<<16)  /* A16 = ALE high */
#endif



#define Bank2_NAND_ADDR    ((u32)0x80000000)
#define Bank_NAND_ADDR     Bank2_NAND_ADDR 

//#define  FLASH_CMD       *(uint8 *)(0x80020000)
//#define  FLASH_ADDR     *(uint8 *)(0x80010000)
#define  FLASH_CMD       *(uint8 *)(0x80010000)
#define  FLASH_ADDR      *(uint8 *)(0x80020000)
#define  FLASH_DATA      *(uint8 *)(0x80000000)

/* Private macro -------------------------------------------------------------*/
#define FlashSetAddress   FlashWriteAddr5Byte
#define FLASH_Ready      ((HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6) &GPIO_PIN_6)== GPIO_PIN_6)
 
 
uint16_t  NAND_PageNums_PerBLOCK;           /*   bytes pages per block */
uint16_t  NAND_BlockNums_PerChip ;         /*   Block per zone */

uint32_t NandFlash_ID;

uint8_t NAND_Dummy_Buf[	NAND_Bytes_PerPAGE ];

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : FSMC_NAND_Init
* Description    : Configures the FSMC and GPIOs to interface with the NAND memory.
*                  This function must be called before any write/read operation
*                  on the NAND.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void FSMC_NAND_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;

//	FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
//	FSMC_NAND_PCCARDTimingInitTypeDef  p;

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);

//	/*-- GPIO Configuration ------------------------------------------------------*/
//	/* CLE, ALE, D0->D3, NOE, NWE and NCE2  NAND pin configuration  */

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);

//	/* D0->D3,*/
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1 , GPIO_AF_FSMC);

//	/* D4->D7 NAND pin configuration  */
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

//	/*-- GPIO Configuration ------------------------------------------------------*/
//	/* CLE, ALE, D0->D7, NOE, NWE and NCE2  NAND pin configuration  */
//	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_14 | GPIO_Pin_15;

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

//	GPIO_Init(GPIOD, &GPIO_InitStructure);


//	/* D4->D7 NAND pin configuration  */
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);


//	/*CLE, ALE */
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12 , GPIO_AF_FSMC);

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	/*NOE, NWE*/
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4 , GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5 , GPIO_AF_FSMC);

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	/*PD7 -> NCE2    PG9 -> NCE3*/
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7 , GPIO_AF_FSMC);
//	//GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
//	//GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//GPIO_Init(GPIOG, &GPIO_InitStructure);

//	/*NWAIT*/
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6 , GPIO_AF_FSMC);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	/*-- FSMC Configuration ------------------------------------------------------*/
//	 if(  SystemCoreClock ==  168000000  )
//	 {
////		p.FSMC_SetupTime = 0x01;
////		p.FSMC_WaitSetupTime = 0x05;
////		p.FSMC_HoldSetupTime = 0x05;
////		p.FSMC_HiZSetupTime = 0x01;
//		 
//		p.FSMC_SetupTime = 0x00;
//		p.FSMC_WaitSetupTime = 0x02;
//		p.FSMC_HoldSetupTime = 0x02;
//		p.FSMC_HiZSetupTime = 0x00;
//	 }
//	 else
//	 {
////		p.FSMC_SetupTime = 0x02;
////		p.FSMC_WaitSetupTime = 0x0a;
////		p.FSMC_HoldSetupTime = 0x0a;
////		p.FSMC_HiZSetupTime = 0x02;
//		p.FSMC_SetupTime = 0x01;
//		p.FSMC_WaitSetupTime = 0x05;
//		p.FSMC_HoldSetupTime = 0x05;
//		p.FSMC_HiZSetupTime = 0x01;
//	 }

//	FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
//	FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
//	FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
//	 
//	 #if NandFlash_ECC_EN>0
//		FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
//	 #else
//		FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Disable;
//	 #endif
//	 
//	FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
//	FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
//	FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
//	FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
//	FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

//	FSMC_NANDInit(&FSMC_NANDInitStructure);

//	/* FSMC NAND Bank Cmd Test */
//	FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);

//}

/********************************************************************
函数功能：FLASH写命令。
入口参数：Val：要写的命令值。
返    回：无。
备    注：
********************************************************************/
void FlashWriteCommand(uint8 Cmd)
{
	*(uint8 *)(Bank_NAND_ADDR | CMD_AREA) = Cmd;
}


/********************************************************************
函数功能：FLASH读数据。
入口参数：Data：要读的数据。
返    回：无。
备    注：
********************************************************************/
uint8 FlashReadDataByte(void)
{
	uint8 Data;
	Data = *(uint8 *)(Bank_NAND_ADDR);
	return  Data;
}

/********************************************************************
函数功能：FLASH写数据。
入口参数：Data：要写的数据。
返    回：无。
备    注：
********************************************************************/
void FlashWriteDataByte(uint8 Data)
{
	*(uint8 *)(Bank_NAND_ADDR )=Data;
}
/********************************************************************
函数功能：FLASH写地址。
入口参数：Addr：要写的地址。
返    回：无。
备    注：
********************************************************************/
void FlashWriteAddr(uint8 Addr)
{
	*(uint8 *)(Bank_NAND_ADDR | ADDR_AREA) = Addr;
}



/********************************************************************
函数功能：等待FLASH操作完成
入口参数：无
返    回：TRUE=未超时  FALSE超时。
备    注：
********************************************************************/
uint8 FlashWait(void)
{	
#if 1
	uint32 i;
	uint32 m;
	for (i = NAND_TIMEOUT; i; i--)
	{
		FlashWriteCommand(NAND_CMD_STATUS);

		for (m=0;m<50;m++);

		//if( FLASH_Ready )
		if ( FlashReadDataByte() & NAND_STAT_RDY )
		{
			for (m=0;m<20;m++);                // 2015-09-24
			return (ERR_NAND_NOERR);
		}
	}
	return (ERR_NAND_HW_TOUT);
#else
	uint32 i;
	uint32 m;
	
	FlashWriteCommand(NAND_CMD_STATUS);
	for (i = NAND_TIMEOUT; i; i--)
	{
		for (m=0;m<50;m++);

		if( FLASH_Ready )
		{
			for (m=0;m<20;m++);                // 2015-09-24
			return (ERR_NAND_NOERR);
		}
	}
	return (ERR_NAND_HW_TOUT);
#endif	
}
/////////////////////////End of function/////////////////////////////
/********************************************************************
函数功能：往FLASH写入5字节地址的函数。
入口参数：Addr：要写入的字节地址。
返    回：无。
备    注：
********************************************************************/
void FlashWriteAddr5Byte(uint32 RowAddr,uint16 ColAddress)
{	
// Col
	FlashWriteAddr(  (uint8)ColAddress );
	FlashWriteAddr(  (uint8)((ColAddress>>8)&0x0f ) );
// Row
	FlashWriteAddr(  (uint8)((RowAddr>>0)) );
	FlashWriteAddr(  (uint8)((RowAddr>>8)) );
	FlashWriteAddr(  (uint8)((RowAddr>>16)&0x07) );
}

/********************************************************************
函数功能：往FLASH写入四字节地址的函数。
入口参数：Addr：要写入的字节地址。
返    回：无。
备    注：
********************************************************************/
void FlashWriteAddr4Byte(uint32 Addr)
{
	uint i;

	//注意：该FLASH的地址中间有5bit必须置0
	//并将高16位左移5位
	Addr=((Addr<<5)&(~0xFFFF))|(Addr&0x07FF);

	for (i=0;i<4;i++) //写入四字节的地址
	{
		FlashWriteAddr(Addr);
		Addr>>=8;
	}
}
/////////////////////////End of function/////////////////////////////
/********************************************************************
函数功能：往FLASH写入二字节地址的函数。
入口参数：Addr：要写入的字节地址。
返    回：无。
备    注：
********************************************************************/
void FlashWriteAddr2Byte(uint16 Addr)
{
	uint i;

	//只需要低11位
	Addr=Addr&(NAND_Bytes_PerPAGE-1);

	for (i=0;i<2;i++)    //写入二字节的地址
	{
		FlashWriteAddr(Addr);
		Addr>>=8;
	}
}


/********************************************************************
函数功能：往FLASH写入二字节页地址的函数。
入口参数：Addr：要写入的字节地址。
返    回：无。
备    注：
********************************************************************/
void FlashWritePageAddr(uint32 Addr)
{
	uint i;

	//计算页地址
	Addr /= NAND_Bytes_PerPAGE;

	for (i=0;i<2;i++) //写入二字节的地址
	{
		FlashWriteAddr(Addr); //写一字节数据
		Addr>>=8;
	}
}

/********************************************************************
函数功能：往FLASH 的块地址，该地址其实为块内某个页地址
入口参数：Addr：要写入的字节地址。
返    回：无。
备    注：
********************************************************************/
void FlashWriteBlockAddr(uint32 Addr)
{
	uint i;

	//计算 Block 地址
	//Addr /= FLASH_BLOCK_SIZE;

	for (i=0;i<FLASH_BLOCK_ADDR_CYCLE;i++) //写入3字节的地址
	{
		FlashWriteAddr(Addr); //写一字节数据
		Addr>>=8;
	}
}
/////////////////////////End of function////////////

/********************************************************************
函数功能：Flash复位。
入口参数：无。
返    回：无。
备    注：
********************************************************************/
void FlashReset(void)
{
	FlashWriteCommand(NAND_CMD_RESET); //写复位命令
	FlashWait();  //等待命令完成
}
/********************************************************************
函数功能：读FLASH的ID号。
入口参数：Buf：保存ID号的缓冲区。
返    回：无。
备    注：无。
********************************************************************/
void FlashReadId(uint8 *Buf)
{
	uint8 i;
	FlashWriteCommand(NAND_CMD_READID);
	FlashWriteAddr(0); //写地址

	for (i=0;i<5;i++) //读5字节的ID
	{
		Buf[i]=FlashReadDataByte();
	}
}

/********************************************************************
函数功能：Flash初始化。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
void FlashInit(void)
{
	uint8 buf[5],i;
	
	//FSMC_NAND_Init();	//FSMC INIT
	FlashReset(); //FLASH复位
	
	FlashReadId(buf); 
	
	NandFlash_ID =0;
	for(i=0;i<4;i++)
	{
		NandFlash_ID <<= 8;
		NandFlash_ID |=buf[i]; 
	} 
	
	if( NandFlash_ID == K9K8G08_ID )
	{
		NAND_PageNums_PerBLOCK  = ((u16)64 ) ;        /*   bytes pages per block */
		NAND_BlockNums_PerChip = ((u16)8192 ) ;    /*   Block per zone */
		//NAND_Bytes_PerPAGE =     ((u16)2048 ) ;    /*   bytes per page w/o Spare Area */
		//NAND_SPARE_AREA_SIZE  =   ((u16)64 )  ;       /*   bytes in spare area */	
		//NAND_MAX_ZONE    =   ((u16)0x0001);   /* 1 zones   */
		//FLASH_BAD_BLOCKS_MAX =180   ;       //最多允许坏块数量
	}
	else if( NandFlash_ID == MT29F8G08ABABA_ID )
	{
		NAND_PageNums_PerBLOCK  = ((u16)NAND0_PAGE_COUNT ) ;        /*   bytes pages per block */
		NAND_BlockNums_PerChip = ((u16)NAND0_BLOCK_COUNT ) ;    /*   Block per zone */
		//NAND_Bytes_PerPAGE =     ((u16)2048 ) ;    /*   bytes per page w/o Spare Area */
		//NAND_SPARE_AREA_SIZE  =   ((u16)64 )  ;       /*   bytes in spare area */	
		//NAND_MAX_ZONE    =   ((u16)0x0001);   /* 1 zones   */
		//FLASH_BAD_BLOCKS_MAX =180   ;       //最多允许坏块数量
	}	
	else
	{
		NAND_PageNums_PerBLOCK  = ((u16)128 ) ;        /*   bytes pages per block */
		NAND_BlockNums_PerChip = ((u16)4096 ) ;    /*   Block per zone */
		//NAND_Bytes_PerPAGE =     ((u16)2048 ) ;    /*   bytes per page w/o Spare Area */
		//NAND_SPARE_AREA_SIZE  =   ((u16)64 )  ;       /*   bytes in spare area */	
		//NAND_MAX_ZONE    =   ((u16)0x0001);   /* 1 zones   */
		//FLASH_BAD_BLOCKS_MAX =180   ;       //最多允许坏块数量
	} 	
}

/********************************************************************
函数功能：读取状态寄存器。
入口参数：无。
返    回：无。
备    注：FLASH操作状态。
********************************************************************/
uint8 FlashReadStatus(void)
{
	uint8 Status;
	FlashWriteCommand(NAND_CMD_STATUS);  //写查询命令
	Status  = FLASH_DATA;  //读回状态
	return Status;
}
/********************************************************************
函数功能：检查FLASH状态。
入口参数：无。
返    回：无。
备    注：FLASH操作状态。
********************************************************************/
uint8 NAND_ChkStatus (uint8 flag) 
{    
  return (FlashReadStatus() & flag) ? NAND_FLAG_SET : NAND_FLAG_CLR; 
}

/********************************************************************
函数功能：擦除一个块。
入口参数：Addr：块地址。
返    回：无。
备    注：无。
********************************************************************/
uint8 FlashEraseBlock(uint16 Addr)
{
//擦除命令第一字节命令
	FlashWriteCommand(NAND_CMD_ERASE1ST);
//写入块地址
	FlashWriteBlockAddr(Addr*NAND_PageNums_PerBLOCK);
//写入擦除命令第二字节
	FlashWriteCommand(NAND_CMD_ERASE2ND);
//等待擦除完成
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	/* Check status */
	switch (NAND_ChkStatus (NAND_STAT_FAIL)) 
	{
		case NAND_FLAG_TOUT:
		  return ERR_NAND_HW_TOUT;
		case NAND_FLAG_SET:
		  return ERR_NAND_PROG;
		default:
		  return ERR_NAND_NOERR;
	} 
}

/********************************************************************
函数功能：页写命令的后半部分。
入口参数：无。
返    回： 
备    注：操作的状态。
********************************************************************/
uint8 FlashWritePageCMD(void)
{
	FlashWriteCommand(NAND_CMD_PROG2ND);  //页写命令
	FlashWait();  //等待写完成
	return FlashReadStatus();
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：复制一页数据。
入口参数：sAddr：源地址；dAddr：目标地址。
返    回：无。
备    注：操作的状态。
********************************************************************/
uint8 FlashCopyPage(uint32 sAddr, uint32 dAddr)
{
//读源数据到内部缓存
	FlashWriteCommand(NAND_CMD_CopyBackRead1ST);
	FlashSetAddress(sAddr,0);
	FlashWriteCommand(NAND_CMD_CopyBackRead2ND);
	FlashWait();

//将数据写到目标页
	FlashWriteCommand(NAND_CMD_CopyBackPROG1ST);
	FlashSetAddress(dAddr,0);
	FlashWriteCommand(NAND_CMD_CopyBackPROG2ND);
	FlashWait();
	return FlashReadStatus();
}

/********************************************************************
函数功能：写入一页数据，默认为从该页的Col地址为0开始，且写满一页。
入口参数：Addr： 目标地址, pBuf 数据缓冲区。
返    回： 操作的状态。
备    注： 
********************************************************************/
uint8 FlashWriteOnePage(uint32 PageAddr, uint8 * pBuf )
{
	uint16 i;
	#if NandFlash_ECC_EN>0
		uint32  write_ecc;
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,ENABLE);
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG1ST);
	FlashSetAddress(PageAddr,0);
	
	for(i=0;i<NAND_Bytes_PerPAGE;i++)
		FLASH_DATA  = *pBuf++  ;
	
	#if NandFlash_ECC_EN>0
		while(FSMC_GetFlagStatus(FSMC_Bank2_NAND,FSMC_FLAG_FEMPT)==Bit_RESET);
		write_ecc = FSMC_GetECC(FSMC_Bank2_NAND);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);	
	
	    FLASH_DATA  =  0xFF;   // 后移1个字节
		FLASH_DATA  = (uint8_t)(write_ecc>>24);
		FLASH_DATA  = (uint8_t)(write_ecc>>16);
		FLASH_DATA  = (uint8_t)(write_ecc>>8);
		FLASH_DATA  = (uint8_t)(write_ecc);			
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG2ND);
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	/* Check status */
	switch (NAND_ChkStatus (NAND_STAT_FAIL)) 
	{
		case NAND_FLAG_TOUT:
		  return ERR_NAND_HW_TOUT;
		case NAND_FLAG_SET:
		  return ERR_NAND_PROG;
		default:
		  return ERR_NAND_NOERR;
	} 
}

// 测试用
uint8 FlashWriteOnePageWithECC(uint32 PageAddr, uint8 * pBuf ,uint32 *ecc)
{
	uint16 i;
	#if NandFlash_ECC_EN>0
		uint32  write_ecc;
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,ENABLE);
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG1ST);
	FlashSetAddress(PageAddr,0);
	
	for(i=0;i<NAND_Bytes_PerPAGE;i++)
		FLASH_DATA  = *pBuf++  ;
	
	#if NandFlash_ECC_EN>0
		while(FSMC_GetFlagStatus(FSMC_Bank2_NAND,FSMC_FLAG_FEMPT)==Bit_RESET);
		write_ecc = FSMC_GetECC(FSMC_Bank2_NAND);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);	
	
	    FLASH_DATA  =  0xFF;   // 后移1个字节
		FLASH_DATA  = (uint8_t)(write_ecc>>24);
		FLASH_DATA  = (uint8_t)(write_ecc>>16);
		FLASH_DATA  = (uint8_t)(write_ecc>>8);
		FLASH_DATA  = (uint8_t)(write_ecc);			
		*ecc = write_ecc;
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG2ND);
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	/* Check status */
	switch (NAND_ChkStatus (NAND_STAT_FAIL)) 
	{
		case NAND_FLAG_TOUT:
		  return ERR_NAND_HW_TOUT;
		case NAND_FLAG_SET:
		  return ERR_NAND_PROG;
		default:
		  return ERR_NAND_NOERR;
	} 
}

uint8 FlashWriteOnePageWithoutECC(uint32 PageAddr, uint8 * pBuf ,uint32 *ecc)
{
	uint16 i;
	#if 0
		uint32  write_ecc;
			FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,ENABLE);
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG1ST);
	FlashSetAddress(PageAddr,0);
	
	for(i=0;i<NAND_Bytes_PerPAGE;i++)
	//for(i=0;i<2050;i++)	
		FLASH_DATA  = *pBuf++  ;
	
		FLASH_DATA  =  0xFF;   // 后移1个字节
		FLASH_DATA  = (uint8_t)(*ecc>>24);
		FLASH_DATA  = (uint8_t)(*ecc>>16);
		FLASH_DATA  = (uint8_t)(*ecc>>8);
		FLASH_DATA  = (uint8_t)(*ecc);		
	
	#if 0
		while(FSMC_GetFlagStatus(FSMC_Bank2_NAND,FSMC_FLAG_FEMPT)==Bit_RESET);
		write_ecc = FSMC_GetECC(FSMC_Bank2_NAND);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);	
	    FLASH_DATA  =  0xFF;   // 后移1个字节
		FLASH_DATA  = (uint8_t)(write_ecc>>24);
		FLASH_DATA  = (uint8_t)(write_ecc>>16);
		FLASH_DATA  = (uint8_t)(write_ecc>>8);
		FLASH_DATA  = (uint8_t)(write_ecc);	
	#endif
	
	FlashWriteCommand(NAND_CMD_PROG2ND);
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	/* Check status */
	switch (NAND_ChkStatus (NAND_STAT_FAIL)) 
	{
		case NAND_FLAG_TOUT:
		  return ERR_NAND_HW_TOUT;
		case NAND_FLAG_SET:
		  return ERR_NAND_PROG;
		default:
		  return ERR_NAND_NOERR;
	} 
}


/********************************************************************
函数功能：写入一页数据，默认为从该页的Col地址为0开始，写入长度有参数决定，但不可超过一页。
入口参数：Addr： 目标地址, pBuf 数据缓冲区。
返    回： 操作的状态。
备    注： 
********************************************************************/
uint8 FlashWriteOnePageWithSize(uint32 PageAddr, uint8 * pBuf,uint16 size)
{
	uint16 i;
	FlashWriteCommand(NAND_CMD_PROG1ST);
	FlashSetAddress(PageAddr,0);
	
	for(i=0;i<size;i++)	 
		FLASH_DATA  = *pBuf++  ;
	
	FlashWriteCommand(NAND_CMD_PROG2ND);
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	/* Check status */
	switch (NAND_ChkStatus (NAND_STAT_FAIL)) 
	{
		case NAND_FLAG_TOUT:
		  return ERR_NAND_HW_TOUT;
		case NAND_FLAG_SET:
		  return ERR_NAND_PROG;
		default:
		  return ERR_NAND_NOERR;
	} 
}

/********************************************************************
函数功能：根据ECC码修正bit错误，若bit错误太多，则不处理
入口参数：read_ecc：STM32读出来的硬件 ECC校验码值, write_ecc：写入时校验的ECC；pBuf 数据缓冲区,size pBuf的大小。
返    回：错误bit的数量。
备    注：有的时候，读出字节数并不是一个page，也就是pBuf的size不是一个NAND_Bytes_PerPAGE，此时若错误字节在size之外就不校正 
********************************************************************/
uint8  Correct_NandFlash_BitError(uint32 read_ecc,uint32 write_ecc,uint8 *pBuf,uint16 size)
{
		uint32 ECCXOR,ERRBitPostion,i,CountOf1,ERRBytePostion;
		uint8  tmpB;
		uint32 location=0;
	
        ECCXOR=(write_ecc^read_ecc)&NAND_ECC_Bits_MASK;
        //没有错误
        if(ECCXOR==0x0)                
			return 0x0;

        CountOf1=0;
        // 计算NAND_ECC_Bits_Size个bit中1出现的次数 
        for(i=0;i<NAND_ECC_Bits_Size;i++) 
        {
                if(   (ECCXOR&(0x01<<i))   !=  0  ) 
					CountOf1+=1;
        }

        ERRBytePostion=0;
        ERRBitPostion=0;		
		
        //可以校正的错误，进行错误矫正			
        if(CountOf1==(NAND_ECC_Bits_Size/2))
        {
				for(i=0;i<NAND_ECC_Bits_Size/2;i++)
				{
					tmpB=(ECCXOR>>(i*2))&0x3;
					if(tmpB==0x01)
					{
						//0
					}
					else if(tmpB==0x02)
					{
						// 1
						location |= (1<<i) ;
					}
					else 
					{
						return 0;
					}					
				}			
				
				ERRBytePostion = location>>3;       //    /8 = 字节位置
				ERRBitPostion   =    location&0x07;  //  %8 =  bit 位置				
				
				if(size > ERRBytePostion)
					pBuf[ERRBytePostion]=pBuf[ERRBytePostion]^(1<<ERRBitPostion);
							
                return 0x01;
        }
        else return 0x03;  
}

/********************************************************************
函数功能：读出一页数据。
入口参数：Addr： 目标地址, pBuf 数据缓冲区,Size拟读出的字节数。
返    回：操作的状态。
备    注： 
********************************************************************/
uint8 FlashReadOnePage(uint32 PageAddr,uint16 ColAddr, uint8 *pBuf,uint16 Size)
{
	uint16 i;
	uint8 *p;	
	
	#if NandFlash_ECC_EN>0
		uint32  read_ecc,write_ecc;
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,ENABLE);
	    p =pBuf;
	#endif
	
	FlashWriteCommand(NAND_CMD_READ1ST);
	FlashSetAddress(PageAddr,ColAddr);
	FlashWriteCommand(NAND_CMD_READ2ND);
	
	if( FlashWait() == ERR_NAND_HW_TOUT)
		return ERR_NAND_HW_TOUT;
	
	FlashWriteCommand(NAND_CMD_READ1ST);
	
	for(i=0;i<Size;i++)
		*pBuf++= FLASH_DATA;
	
	#if NandFlash_ECC_EN>0	
		for(;i<NAND_Bytes_PerPAGE;i++)
			NAND_Dummy_Buf[i]=FLASH_DATA;

	    while(FSMC_GetFlagStatus(FSMC_Bank2_NAND,FSMC_FLAG_FEMPT)==Bit_RESET);
		read_ecc = FSMC_GetECC(FSMC_Bank2_NAND);
		FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);
	
		write_ecc = FLASH_DATA;    // 略过
		write_ecc = FLASH_DATA;    write_ecc  <<= 8;
		write_ecc |= FLASH_DATA;   write_ecc  <<= 8;
		write_ecc |= FLASH_DATA;   write_ecc  <<= 8;
		write_ecc |= FLASH_DATA; 
		
	    read_ecc = read_ecc & NAND_ECC_Bits_MASK;
	    write_ecc = write_ecc & NAND_ECC_Bits_MASK;	
        
		Correct_NandFlash_BitError(read_ecc,write_ecc,p,Size);		 	
	#endif
		
	return ERR_NAND_NOERR;	
}
/////////////////////////End of  function/////////////////////////////

#if 0
 
#endif
/////////////////////////End of function/////////////////////////////
/********************************************************************
函数功能：查看出厂标记的坏块地址和数量。
入口参数： 
返    回： 坏块的块地址编号及坏块数量，若坏块数量多于最多允许值，则返回FALSE，表明FLASH无法满足需求。
备    注： 。
********************************************************************/
uint8 Check_NAND_InvalidBlocks(uint16 *InvalidBlockIndex,uint16 *InvalidBlockCount)
{
	uint32 i;
	 
	uint8  Buf[2],InfByte[2];
	uint32 RowAddr,ColAddr=2048;
	uint16 Invalid=0;
	
	if( NandFlash_ID == K9K8G08_ID )
	{
		RowAddr = 0 ; // 第1page的2048字节	
		ColAddr=2048;		
	}
	else
	{
		RowAddr =NAND_PageNums_PerBLOCK-1 ; // 最后page的2048字节	
		ColAddr=2048;	
	}
	
	// #ifndef  NandFlash_LDMF1GA
		// RowAddr = 0 ; // 第1page的2048字节	
		// ColAddr=2048;
	// #else
		// RowAddr =NAND_PageNums_PerBLOCK-1 ; // 最后page的2048字节	
		// ColAddr=2048;	
	// #endif	
	
	for(i=0;i<NAND_BlockNums_PerChip;i++)
	{
		if( NandFlash_ID == K9K8G08_ID )
		{
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[0] = Buf[0];
			RowAddr ++ ;
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[1] = Buf[0];
			if( (InfByte[0]!=0xff)||(InfByte[1]!=0xff))
			{
				InvalidBlockIndex[Invalid]=i;
				
				Invalid++;
				if(Invalid > FLASH_BAD_BLOCKS_MAX)
					return ERR_NAND_TooManyBadBlock;			 
			}
			RowAddr  += NAND_PageNums_PerBLOCK-1 ;				
		}
		else
		{
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[0] = Buf[0];
			
			if( (InfByte[0]!=0xff))
			{
				InvalidBlockIndex[Invalid]=i;
				
				Invalid++;
				if(Invalid > FLASH_BAD_BLOCKS_MAX)
					return ERR_NAND_TooManyBadBlock;			 
			}
			RowAddr  += NAND_PageNums_PerBLOCK ;
		}
		/*
		#ifdef  NandFlash_LDMF1GA
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[0] = Buf[0];
			
			if( (InfByte[0]!=0xff))
			{
				InvalidBlockIndex[Invalid]=i;
				
				Invalid++;
				if(Invalid > FLASH_BAD_BLOCKS_MAX)
					return ERR_NAND_TooManyBadBlock;			 
			}
			RowAddr  += NAND_PageNums_PerBLOCK ;
		#else
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[0] = Buf[0];
			RowAddr ++ ;
			FlashReadOnePage(RowAddr,ColAddr,Buf,2);
			InfByte[1] = Buf[0];
			if( (InfByte[0]!=0xff)||(InfByte[1]!=0xff))
			{
				InvalidBlockIndex[Invalid]=i;
				
				Invalid++;
				if(Invalid > FLASH_BAD_BLOCKS_MAX)
					return ERR_NAND_TooManyBadBlock;			 
			}
			RowAddr  += NAND_PageNums_PerBLOCK-1 ;	
		#endif
		*/
	}
	  
	*InvalidBlockCount=Invalid;
	
	return ERR_NAND_NOERR;	
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

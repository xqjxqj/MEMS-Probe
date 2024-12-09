/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : fsmc_nand.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Header for fsmc_nand.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FSMC_NAND_H
#define __FSMC_NAND_H

/* Includes ------------------------------------------------------------------*/
//#include <stm32f4xx.h>   
//#include "main.h"
//my type
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long int
#define int8     signed char
#define int16    signed short int
#define int32    signed long int
#define uint64   unsigned long long int
#define int64    signed long long int
	
#define u8     unsigned char
#define u16    unsigned short 
#define u32    unsigned long
	
#define uint unsigned int

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
 
/* FSMC NAND memory parameters */

#define NandFlash_LDMF1GA
#define NandFlash_ECC_EN          0
#define NAND_ECC_Bits_Size   28       // STM32硬件 ECC校验使用的位宽
#define NAND_ECC_Bits_COUNT   16383       // STM32硬件 ECC校验使用的位宽最大能表示的值
#define NAND_ECC_Bits_MASK    0xFFFFFFF


#ifndef  NandFlash_LDMF1GA
	#define NAND_Bytes_PerPAGE             ((u16)2048 )     /*   bytes per page w/o Spare Area */
	//#define NAND_PageNums_PerBLOCK    ((u16)64 )         /*   bytes pages per block */
	//#define NAND_BlockNums_PerChip       ((u16)8192 )     /*   Block per zone */
	#define NAND_SPARE_AREA_SIZE       ((u16)64 )         /*   bytes in spare area */
	#define NAND_MAX_ZONE                   ((u16)0x0001)   /* 1 zones   */
	
	//最多允许坏块数量
	#define FLASH_BAD_BLOCKS_MAX  180

#else
	#define NAND_Bytes_PerPAGE              ((u16)2048 )     /*   bytes per page w/o Spare Area */
	//#define NAND_PageNums_PerBLOCK     ((u16)128 )         /*   bytes pages per block */
	//#define NAND_BlockNums_PerChip        ((u16)4096 )     /*   Block per zone */
	#define NAND_SPARE_AREA_SIZE         ((u16)64 )         /*   bytes in spare area */
	#define NAND_MAX_ZONE                     ((u16)0x0001)   /* 1 zones   */
	
	//最多允许坏块数量
	#define FLASH_BAD_BLOCKS_MAX 180
	
#endif
 
extern uint16_t  NAND_PageNums_PerBLOCK  ;       /*   bytes pages per block */
extern uint16_t  NAND_BlockNums_PerChip ;    /*   Block per zone */

extern uint32_t NandFlash_ID;

#define K9K8G08_ID   0xECD35195
#define K9G8G08_ID   0xECD314A5
#define LDMF1GA_ID   0xECD314A5	
#define MT29F8G08ABABA_ID   0x2C380026	


//FLASH最大能够达到的地址 
#define FLASH_MAX_ADDR    0x3FFFFFFF     // 1G bytes

//FLASH块大小 
#define FLASH_BLOCK_SIZE  (NAND_PageNums_PerBLOCK*NAND_Bytes_PerPAGE)

#define FLASH_BYTE_ADDR_CYCLE    5
#define FLASH_BLOCK_ADDR_CYCLE  3

//FLASH页大小 
//#define FLASH_PAGE_SIZE    NAND_PAGE_SIZE

//扇区大小
#define FLASH_SECTOR_SIZE 0x200

/* NAND Flash Commands (ONFI V1.0 and V2.2 mandatory) */
#define NAND_CMD_READ1ST      0x00      /* Read 1st Cycle                    */
#define NAND_CMD_CHRDCOL1ST   0x05      /* Change Read Column 1st Cycle      */
#define NAND_CMD_PROG2ND      0x10      /* Page Program 2nd Cycle            */
#define NAND_CMD_READ2ND      0x30      /* Read 2nd Cycle                    */
#define NAND_CMD_ERASE1ST     0x60      /* Block erase 1st Cycle             */
//#define NAND_CMD_STATUS       0x70      /* Read Status                       */
#define NAND_CMD_PROG1ST      0x80      /* Page Program 1st Cycle            */
#define NAND_CMD_CHWRCOL      0x85      /* Change Write Column               */
//#define NAND_CMD_READID       0x90      /* Read ID                           */
#define NAND_CMD_ERASE2ND     0xD0      /* Block erase 2nd cycle             */
#define NAND_CMD_CHRDCOL2ND   0xE0      /* Change Read Column 2nd Cycle      */
#define NAND_CMD_PARAMPAGE    0xEC      /* Read Parameter Page               */
//#define NAND_CMD_RESET        0xFF      /* Reset command                     */

#define NAND_CMD_RandomREAD1ST      0x05      /* Random Read 1st Cycle                    */
#define NAND_CMD_RandomREAD2ND      0xE0      /* Random Read 2nd Cycle                    */

#define NAND_CMD_CopyBackRead1ST        0x00      /*  Copy-Back Read 1st Cycle                 */
#define NAND_CMD_CopyBackRead2ND       0x35      /*  Copy-Back Read 2nd Cycle                 */
#define NAND_CMD_CopyBackPROG1ST       0x85      /*  Copy-Back Prog 1st Cycle                 */
#define NAND_CMD_CopyBackPROG2ND      0x10      /*  Copy-Back Prog 2nd Cycle                 */

/* NAND Status Flags Masks (ONFI V1.0 and V2.2) */
#define NAND_STAT_FAIL        0x01      /* Last command failed               */
#define NAND_STAT_FAILC       0x02      /* Command prior last failed         */
#define NAND_STAT_CSP         0x08      /* Command specific                  */
#define NAND_STAT_VSP         0x10      /* Vendor specific                   */
#define NAND_STAT_ARDY        0x20      /* Array operation in progress       */
#define NAND_STAT_RDY         0x40      /* LUN ready for another command     */
#define NAND_STAT_WP          0x80      /* Write protected                   */


#define NAND_TIMEOUT  20000   // 20000

/* Flag Set/Cleared */
#define NAND_FLAG_CLR       0x00
#define NAND_FLAG_SET       0x01

#define NAND_FLAG_TOUT     0x02

/* NFTL Error Codes */
typedef enum {
  ERR_NAND_NOERR = 0,                   /* No error                          */ 

  ERR_NAND_PROG,                        /* Programming failed (flash status) */
  ERR_NAND_ERASE,                       /* Erase verify failed (flash status)*/
  ERR_NAND_HW_TOUT,                     /* NAND hardware timeout             */
  ERR_NAND_DMA_TOUT,                    /* DMA timeout                       */
  ERR_NAND_TooManyBadBlock,             /* too many bad block   */
  ERR_NAND_UNSUPPORTED               /* Functionality not supported       */
} NAND_ERROR_TYPE;

void FSMC_NAND_Init(void);
void FlashReset(void);
void FlashReadId(uint8 *Buf);
void FlashInit(void);
uint8 FlashEraseBlock(uint16 Addr);
uint8 FlashCopyPage(uint32 sAddr, uint32 dAddr);
uint8 FlashWriteOnePage(uint32 PageAddr, uint8 * pBuf);
uint8 FlashWriteOnePageWithSize(uint32 PageAddr, uint8 * pBuf,uint16 size);
uint8 FlashReadOnePage(uint32 PageAddr,uint16 ColAddr, uint8 *pBuf,uint16 Size);
uint8 Check_NAND_InvalidBlocks(uint16 *InvalidBlockIndex,uint16 *InvalidBlockCount);
 

/********************************************************************
函数功能：标志当前交换块为坏块。
入口参数：无。
返    回：下一个可用的交换块的地址。
备    注：无。
********************************************************************/
 
/////////////////////////End of function/////////////////////////////

extern uint8 FlashSectorBuf[FLASH_SECTOR_SIZE];


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#endif /* __FSMC_NAND_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

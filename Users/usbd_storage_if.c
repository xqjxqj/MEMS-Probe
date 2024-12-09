/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v3.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define USE_FILE_SYSTEM   1 

#if (USE_FILE_SYSTEM == 1)              // If File System is used
  #include "rl_fs.h"
  
  #define MEDIA_DRIVE "N0"
  
  #define  MEDIA_OWN_USB     (1U     )    // Media owned by USB (bit mask)
  #define  MEDIA_OWN_CHG     (1U << 1)    // Media ownership change requested (bit mask)
  extern 
  volatile uint8_t  usbd_msc0_media_own;
  volatile uint8_t  usbd_msc0_media_own;  // USB MSC0 media ownership
  static   int32_t  drv_id;               // FAT drive id
  static   bool     media_ok;             // Media is initialized and ok
#else
  static   uint32_t memory   [8192];    // Memory in RAM for dummy disk image
  static   uint32_t block_buf[ 512/4];    // Buffer for block read/write to media
  extern
  const    uint8_t  memory_disk_image[4096];  // Dummy Memory Disk Image
#endif

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  0x10000
#define STORAGE_BLK_SIZ                  0x200

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_FS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_FS[] = {/* 36 */

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,
  0x00,
  'I', 'G', 'G', 'C', 'A', 'S', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
};
/* USER CODE END INQUIRY_DATA_FS */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init_FS(uint8_t lun)
{
  /* USER CODE BEGIN 2 */
  #if (USE_FILE_SYSTEM == 1)              // If File System is used
  uint32_t param_status;
 
  usbd_msc0_media_own = MEDIA_OWN_USB;  // Initially media is owned by USB
  media_ok            = false;          // Current media status (not initialized = not ok)
 
  if (finit (MEDIA_DRIVE) != fsOK) {    // Initialize File System
    return USBD_FAIL;                             // Exit if failed
  }
 
  drv_id = fs_ioc_get_id (MEDIA_DRIVE); // Get ID of media drive
  if (drv_id < 0)           { return USBD_FAIL; } // If ID is invalid exit
 
  param_status = 0U;                    // Parameter for function call is 0
                                        // Initialize media
  if (fs_ioc_device_ctrl (drv_id, fsDevCtrlCodeControlMedia, &param_status) != fsOK) {
    return USBD_FAIL;                             // Exit if failed
  }
 
  if (fs_ioc_lock (drv_id)) {           // Lock media for USB usage
    return USBD_FAIL;                             // Exit if failed
  }
 
  media_ok = true;                      // Media was initialized and is ok
#else
                                        // Copy the dummy image from code to RAM
  //memcpy (memory, memory_disk_image, sizeof(memory_disk_image));
  memset (memory, 0x00, sizeof(memory));
#endif

  return (USBD_OK);
  /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
#if (USE_FILE_SYSTEM == 1)              // If File System is used
  fsMediaInfo media_info;
 
  // Read media information of actual media
  if (fs_ioc_read_info(drv_id, &media_info) != fsOK) {
    return USBD_FAIL;                       // Exit if failed
  }
 
  *block_num = media_info.block_cnt;  // Total number of blocks on media
  *block_size  = media_info.read_blen;  // Block size of blocks on media
#else
  *block_num = sizeof(memory)/512U;   // Total number of blocks on media
  *block_size  = 512U;                  // Block size of blocks on media
#endif

  // *block_num  = STORAGE_BLK_NBR;
  // *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady_FS(uint8_t lun)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
#if (USE_FILE_SYSTEM == 1)              // If File System is used
                                        // Read data directly from media
  if (fs_ioc_read_sector (drv_id, blk_addr, buf, blk_len) != fsOK) {
    return USBD_FAIL;
  }
#else
                                        // Read data from dummy image in RAM
  memcpy (buf, (uint8_t *)(&memory[blk_addr * (512U/4U)]), blk_len * 512U);
#endif


  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
#if (USE_FILE_SYSTEM == 1)              // If File System is used
                                        // Write data directly to media
  if (fs_ioc_write_sector (drv_id, blk_addr, buf, blk_len) != fsOK) {
    return false;
  }

#else
                                        // Write data to image in RAM
  memcpy ( (uint8_t *)(&memory[blk_addr * (512U/4U)]), buf, blk_len * 512U);

#endif

  return (USBD_OK);
  /* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun_FS(void)
{
  /* USER CODE BEGIN 8 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */


/**
 * @file Untitled-1
 * @brief 
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-04-27
 * @copyright Copyright (c) 2022  CAS 
 */
#ifndef __IIR_filter_H_
#define __IIR_filter_H_


/* Includes ------------------------------------------------------------------*/


/* Public define --------------------------------------------------------—----*/


/* Typedef ----------------------------------------------------------————————-*/


/* Public macro -------------------------------------------------------------*/


/* Public variables ---------------------------------------------------------*/


/* Public functions ---------------------------------------------------------*/

extern void iir_reset(void);

extern void RawData_LPF(uint8_t *pIn24b, int16_t *pOut16b);

extern void RawData_24b_16b(uint8_t *pIn24BitDataBuf, int16_t *pOut16BitDataBuf, uint8_t ChSize, uint16_t RecordSize);



#endif	 // __IIR_filter_H_ 

/*********** (C) COPYRIGHT 2022 skyhigh *****END OF FILE****/

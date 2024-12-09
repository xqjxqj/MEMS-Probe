/**
 * @file Untitled-1
 * @brief
 * @author yangskyhigh (yangskyhigh@163.com)
 * @version 1.0
 * @date 2022-04-27
 * @copyright Copyright (c) 2022  CAS
 */
#ifndef __IIR_coefs_H_
#define __IIR_coefs_H_

/* Includes ------------------------------------------------------------------*/

/* Public define --------------------------------------------------------—----*/

/* Typedef ----------------------------------------------------------————————-*/

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/
//------------------------- Raw Data  fs=200Hz fp=9Hz fs=32Hz ---------------------------
#define RawData_NSEC 5
const int NL[RawData_NSEC] = {1, 3, 1, 3, 1};
const float RawData_IIR_B[RawData_NSEC][3] = {
    {0.02598145190428, 0, 0},
    {1, 2, 1},
    {0.02236385339992, 0, 0},
    {1, 2, 1},
    {1, 0, 0}};
const int DL[RawData_NSEC] = {1, 3, 1, 3, 1};
const float RawData_IIR_A[RawData_NSEC][3] = {
    {1, 0, 0},
    {1, -1.66730964199, 0.7712354496069},
    {1, 0, 0},
    {1, -1.435157224589, 0.5246126381883},
    {1, 0, 0}};

/* Public functions ---------------------------------------------------------*/

#endif // __IIR_coefs_H_

/*********** (C) COPYRIGHT 2022 skyhigh *****END OF FILE****/

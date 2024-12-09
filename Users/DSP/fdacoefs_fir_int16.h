/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.11 and DSP System Toolbox 9.13.
 * Generated on: 27-Apr-2022 16:30:13
 */

/*
 * 离散时间 FIR 滤波器(实数)
 * ----------------
 * 滤波器结构    : 直接型 FIR
 * 滤波器长度    : 29
 * 稳定       : 是
 * 线性相位     : 是 (Type 1)
 * 算术       : fixed
 * 分子       : s16,17 -> [-2.500000e-01 2.500000e-01)
 * 输入       : s16,13 -> [-4 4)
 * 滤波器内部选项  : Full Precision
 *   输出     : s34,30 -> [-8 8)  (auto determined)
 *   乘积     : s31,30 -> [-1 1)  (auto determined)
 *   累加器    : s34,30 -> [-8 8)  (auto determined)
 *   取整模式   : No rounding
 *   溢出模式   : No overflow
 */

/* General type conversion for MATLAB generated C-code  */
#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * D:\MATLAB\R2021b\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   int16 filter coefficients.
 */
const int BL = 29;
const int16_T FirFilter_B[29] = {
     -339,   -427,   -587,   -668,   -596,   -298,    281,   1159,   2308,
     3651,   5064,   6395,   7489,   8208,   8459,   8208,   7489,   6395,
     5064,   3651,   2308,   1159,    281,   -298,   -596,   -668,   -587,
     -427,   -339
};
